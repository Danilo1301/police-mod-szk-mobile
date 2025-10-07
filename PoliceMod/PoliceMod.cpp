#include "PoliceMod.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Peds.h"
#include "Pullover.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "Audios.h"
#include "windows/TestWindow.h"
#include "windows/RadioWindow.h"
#include "BottomMessage.h"
#include "Objectives.h"
#include "dialog/DialogManager.h"
#include "Chase.h"
#include "BackupUnits.h"
#include "AIController.h"
#include "Criminals.h"
#include "SpriteUtils.h"
#include "Scorch.h"
#include "Checkpoint.h"
#include "PoliceBases.h"

bool hasLoadedAnimations = false;

PoliceMod* policeMod = new PoliceMod();

bool PoliceMod::m_IsUsingMenu = false;

PoliceMod::PoliceMod()
{
}
void PoliceMod::OnModLoad()
{
    menuSZK->onPedAdded->Add([](int ref) {
        auto ptr = menuSZK->GetCPedFromRef(ref);
        Peds::AddPed(ref, ptr);
    });
    
    menuSZK->onPedRemoved->Add([](int ref) {
        Peds::RemovePed(ref);
    });

    menuSZK->onVehicleAdded->Add([](int ref) {
        auto ptr = menuSZK->GetCVehicleFromRef(ref);
        Vehicles::AddVehicle(ref, ptr);
    });
    
    menuSZK->onVehicleRemoved->Add([](int ref) {
        Vehicles::RemoveVehicle(ref);
    });
}

void PoliceMod::Initialize()
{
    Log::canLogDebug = menuSZK->debugMode;

    menuSZK->onUpdate->Add([this]() {
        Update();

        auto ped = Peds::GetPed(GetPlayerActor());
    });

    logInternal("Initialize Pullover");
    Pullover::Initialize();

    logInternal("Initialize BottomMessage");
    BottomMessage::Initialize();

    logInternal("Initialize DialogManager");
    DialogManager::Initialize();

    logInternal("Initialize Objectives");
    Objectives::Initialize();

    logInternal("Initialize RadioWindow");
    RadioWindow::Initialize();

    logInternal("Initialize PoliceBases");
    PoliceBases::Initialize();

    auto widgetTestMenu = menuSZK->CreateWidgetButton(600 + 150, 30, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_vest.png"));
    widgetTestMenu->onClickWidget->Add([]() {
        TestWindow::ShowWindow();
    });

    auto widgetRadio = menuSZK->CreateWidgetButton(600 + 300, 30, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_radio.png"));
    widgetRadio->onClickWidget->Add([]() {

        RadioWindow::Toggle();

    });

    logInternal("Create audios");
    Audios::CreateAudios();
}

void PoliceMod::Update()
{
    logDebug("PoliceMod::Update");

    if(!hasLoadedAnimations && PLAYER_DEFINED(0))
    {
        if(
            HAS_ANIMATION_LOADED("POLICE") &&
            HAS_ANIMATION_LOADED("GANGS") &&
            HAS_ANIMATION_LOADED("MEDIC")
        )
        {
            hasLoadedAnimations = true;

            menuSZK->GetDebug()->AddLine("Animations loaded");
        } else {
            LOAD_ANIMATION("GANGS");
            LOAD_ANIMATION("POLICE");
            LOAD_ANIMATION("MEDIC");
        }
    }

    Peds::Update();
    //logDebug("Vehicles::Update");
    Vehicles::Update();
    //logDebug("Pullover::Update");
    Pullover::Update();
    //logDebug("Dialogs::Update");
    BottomMessage::Update();
    DialogManager::Update();
    Objectives::Update();
    //logDebug("Chase::Update");
    Chase::Update();
    //logDebug("Criminals::Update");
    Criminals::Update();
    //logDebug("BackupUnits::Update");
    BackupUnits::Update();
    //logDebug("AIController::Update");
    AIController::Update();
    Scorch::Update();
    PoliceBases::Update();

    Checkpoints::Update();

    CleoFunctions::Update(menuSZK->deltaTime);
    
    logDebug("PoliceMod::Update END");
}

void PoliceMod::OnDrawRadar()
{
    BackupUnits::OnDrawRadar();
    PoliceBases::OnDrawRadar();
}

void PoliceMod::OnRender()
{
    SpriteUtils::OnRender();

    for(auto pair : Vehicles::GetVehiclesMap())
    {
        auto vehicle = pair.second;
        if(!Vehicles::IsValid(vehicle)) continue;
        auto position = vehicle->GetPosition();

        if(vehicle->mapIconColor.a == 0) continue;

        if(vehicle->IsPlayerInside()) continue;

        SpriteUtils::DrawSpriteInRadarWorld(SpriteUtils::spriteCircle, position, vehicle->mapIconColor, 35.0f);
        SpriteUtils::DrawBlip(position + CVector(0, 0, 2.0f), vehicle->mapIconColor);
    }
    
    auto peds = Peds::GetPedsMap();
    for(auto pair : peds)
    {
        auto ped = pair.second;

        if(!Peds::IsValid(ped)) continue;

        if(IS_CHAR_IN_ANY_CAR(ped->ref)) continue;

        auto position = ped->GetPosition();

        if(ped->mapIconColor.a == 0) continue;

        SpriteUtils::DrawSpriteInRadarWorld(SpriteUtils::spriteCircle, position, ped->mapIconColor, 20.0f);
        SpriteUtils::DrawBlip(position + CVector(0, 0, 1.5f), ped->mapIconColor);
    }

    PoliceBases::OnRender();
}

void PoliceMod::TestEquip()
{
    int playerActor = GET_PLAYER_ACTOR(0);

    GIVE_ACTOR_WEAPON(playerActor, 31, 800);
    GIVE_ACTOR_WEAPON(playerActor, 22, 200);
    // if(!Ped::PedHasWeaponId(playerActor, 10))
    // {
    //     CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
    // }
    CHANGE_PLAYER_MODEL_TO(0, 280);
}