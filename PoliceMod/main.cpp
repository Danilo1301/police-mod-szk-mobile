#include "pch.h"

#include "PoliceMod.h"
#include "Vehicles.h"
#include "Peds.h"
#include "MapIcons.h"

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6
#include "isautils.h"
ISAUtils* sautils = NULL;

// MenuSZK
#include "menuSZK/IMenuSZK.h"
IMenuSZK* menuSZK = NULL;

MYMODCFG(net.daniloszk.policemod, Police Mod SZK, 0.3.0, DaniloSZK)

// --------------------------------------

bool *m_UserPause, *m_CodePause;
float flMenuMapScaling;
RsGlobalType* RsGlobal;

bool IsGamePaused() { return *m_CodePause || *m_UserPause; };

RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
char* (*GetFrameNodeName)(RwFrame* frame);
void (*CSprite2d_DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
void (*TransformRealWorldPointToRadarSpace)(CVector2D& out, CVector2D const& in);
void (*TransformRadarPointToScreenSpace)(CVector2D& out, CVector2D const& in);
void (*LimitRadarPoint)(CVector2D& in);

DECL_HOOKv(InitRenderWare)
{
    InitRenderWare();

    flMenuMapScaling = (float)RsGlobal->maximumHeight / 448.0f;
}

DECL_HOOKv(RenderVehicle, void* self)
{
    CVehicle* pVehicle = (CVehicle*)self;

    auto vehicle = Vehicles::GetVehicle(pVehicle);

    if(vehicle) vehicle->OnRenderBefore();
 
    RenderVehicle(self);

    if(vehicle) vehicle->OnRenderAfter();
}

DECL_HOOKv(PostRadarDraw, bool b)
{
    PostRadarDraw(b);

    policeMod->OnDrawRadar();

    // if(!spriteCircle->loaded) return;

    for(auto pair : Vehicles::GetVehiclesMap())
    {
        auto vehicle = pair.second;
        if(!Vehicles::IsValid(vehicle)) continue;
        auto position = vehicle->GetPosition();

        if(vehicle->IsPoliceCar())
        {
            DrawSpriteInRadar(spriteCircle, position, CRGBA(0, 150, 255), 35.0f);
        } else {
            if(vehicle->IsInChase())
            {
                DrawSpriteInRadar(spriteCircle, position, CRGBA(255, 80, 80), 35.0f);
            } else {
                //DrawSpriteInRadar(spriteCircle, position, CRGBA(255, 255, 255), 30.0f);
            }
        }
    }
    
    auto peds = Peds::GetPedsMap();
    for(auto pair : peds)
    {
        auto ped = pair.second;

        if(!Peds::IsValid(ped)) continue;

        auto position = ped->GetPosition();

        if(ped->IsCriminal())
        {
            DrawSpriteInRadar(spriteCircle, position, CRGBA(255, 80, 80), 18.0f);
        }
    }
}

extern "C" void OnModPreLoad()
{
    char modFolder[512];
    sprintf(modFolder, "%s/policeMod", aml->GetConfigPath());
    
    createDirectoryIfNotExists(modFolder);

    Log::Init(modFolder);
    Log::file->clearLog();
    Log::internal->clearLog();

    logToFile("Log initialized");
    logInternal("Internal log initialized");

    logToFile("Mod preload...");
}

extern "C" void OnModLoad()
{
    logToFile("Mod load...");

    loadInterface(&cleo, "CLEO");
    if(!cleo) return;

    loadInterface(&menuSZK, "MenuSZK");
    if(!menuSZK) return;

    loadInterface(&sautils, "SAUtils");
    if(!sautils) return;

    logToFile("Finding addresses...");

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)cleo->GetMainLibraryLoadAddress();
    uintptr_t pGTASA = aml->GetLib("libGTASA.so");

    HOOKPLT(InitRenderWare, pGTASA + 0x66F2D0);
    HOOK(RenderVehicle, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    HOOK(PostRadarDraw, aml->GetSym(hGTASA, "_ZN6CRadar20DrawRadarGangOverlayEb"));

    SET_TO(RpClumpForAllAtomics, aml->GetSym(hGTASA, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    SET_TO(GetFrameNodeName, aml->GetSym(hGTASA, "_Z16GetFrameNodeNameP7RwFrame"));
    SET_TO(CSprite2d_DrawSprite, aml->GetSym(hGTASA, "_ZN9CSprite2d4DrawERK5CRectRK5CRGBA"));
    SET_TO(TransformRealWorldPointToRadarSpace, aml->GetSym(hGTASA, "_ZN6CRadar35TransformRealWorldPointToRadarSpaceER9CVector2DRKS0_"));
    SET_TO(TransformRadarPointToScreenSpace, aml->GetSym(hGTASA, "_ZN6CRadar32TransformRadarPointToScreenSpaceER9CVector2DRKS0_"));
    SET_TO(m_CodePause, aml->GetSym(hGTASA, "_ZN6CTimer11m_CodePauseE"));
    SET_TO(m_UserPause, aml->GetSym(hGTASA, "_ZN6CTimer11m_UserPauseE"));
    SET_TO(RsGlobal, aml->GetSym(hGTASA, "RsGlobal"));
    SET_TO(LimitRadarPoint, aml->GetSym(hGTASA, "_ZN6CRadar15LimitRadarPointER9CVector2D"));

    spriteCircle = menuSZK->LoadSprite(getPathFromAssets("map/circle.png"));
    spriteBigCircle = menuSZK->LoadSprite(getPathFromAssets("map/big_circle.png"));

    policeMod->OnModLoad();

    menuSZK->onFirstUpdate->Add([]() {
        policeMod->Initialize();
    });
}
