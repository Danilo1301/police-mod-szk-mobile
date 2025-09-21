#include "PoliceMod.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Peds.h"
#include "Pullover.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "Audios.h"
#include "windows/TestWindow.h"
#include "Dialogs.h"
#include "Objectives.h"
#include "dialog/DialogManager.h"

bool hasLoadedAnimations = false;

PoliceMod* policeMod = new PoliceMod();

bool PoliceMod::m_IsUsingMenu = false;

PoliceMod::PoliceMod()
{
}

void PoliceMod::Initialize()
{
    menuSZK->onUpdate->Add([this]() {
        Update();
    });

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

    Pullover::Initialize();
    Dialogs::Initialize();
    DialogManager::Initialize();
    Objectives::Initialize();

    auto widgetTestMenu = menuSZK->CreateWidgetButton(600 + 150, 30, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_vest.png"));
    widgetTestMenu->onClickWidget->Add([]() {
        TestWindow::ShowWindow();
    });

    Audios::CreateAudios();
}

void PoliceMod::Update()
{
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
    Vehicles::Update();
    Pullover::Update();
    Dialogs::Update();
    DialogManager::Update();
    Objectives::Update();

    CleoFunctions::Update(menuSZK->deltaTime);
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