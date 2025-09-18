#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "rw/rwcore.h"

#include "simpleGta.h"
#include "utils.h"

#include "Log.h"
#include "PoliceMod.h"
#include "Vehicles.h"

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6
#include "isautils.h"
ISAUtils* sautils = NULL;

// MenuSZK
#include "menuSZK/IMenuSZK.h"
IMenuSZK* menuSZK = NULL;

MYMODCFG(net.daniloszk.policemod, Police Mod SZK, 0.1.0, DaniloSZK)

// --------------------------------------

RpClump* (*RpClumpForAllAtomics)(RpClump* clump, RpAtomicCallBack callback, void* pData);
RpGeometry* (*RpGeometryForAllMaterials)(RpGeometry* geometry, RpMaterialCallBack fpCallBack, void* pData);
char* (*GetFrameNodeName)(RwFrame* frame);

DECL_HOOKv(RenderVehicle, void* self)
{
    CVehicle* pVehicle = (CVehicle*)self;

    auto vehicle = Vehicles::GetVehicle(pVehicle);

    if(vehicle)
    {
        vehicle->OnRenderBefore();
    }
 
    RenderVehicle(self);

    if(vehicle)
    {
        vehicle->OnRenderAfter();
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

    HOOK(RenderVehicle, aml->GetSym(hGTASA, "_ZN8CVehicle6RenderEv"));
    
    SET_TO(RpClumpForAllAtomics, aml->GetSym(hGTASA, "_Z20RpClumpForAllAtomicsP7RpClumpPFP8RpAtomicS2_PvES3_"));
    SET_TO(RpGeometryForAllMaterials, aml->GetSym(hGTASA, "_Z25RpGeometryForAllMaterialsP10RpGeometryPFP10RpMaterialS2_PvES3_"));
    SET_TO(GetFrameNodeName, aml->GetSym(hGTASA, "_Z16GetFrameNodeNameP7RwFrame"));

    menuSZK->onFirstUpdate->Add([]() {
        policeMod->Initialize();
    });
}
