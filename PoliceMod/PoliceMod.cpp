#include "PoliceMod.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Peds.h"
#include "Pullover.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "Audios.h"

bool hasLoadedAnimations = false;

PoliceMod* policeMod = new PoliceMod();

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

    auto widget = menuSZK->CreateWidgetButton(350, 500, getPathFromMenuAssets("widget_background1.png"), getPathFromMenuAssets("widget_menu.png"));
    widget->onClickWidget->Add([]() {
        menuSZK->CreateTestWindow();
    });

    auto widgetEquip = menuSZK->CreateWidgetButton(350, 650, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_vest.png"));
    widgetEquip->onClickWidget->Add([this, widgetEquip]() {
        TestEquip();
        
        widgetEquip->Destroy();
    });

    Audios::CreateAudios();
}

void PoliceMod::Update()
{
    logInternal("PoliceMod: Update");

    if(!hasLoadedAnimations && CleoFunctions::PLAYER_DEFINED(0))
    {
        if(
            CleoFunctions::HAS_ANIMATION_LOADED("POLICE") &&
            CleoFunctions::HAS_ANIMATION_LOADED("GANGS") &&
            CleoFunctions::HAS_ANIMATION_LOADED("MEDIC")
        )
        {
            hasLoadedAnimations = true;

            menuSZK->GetDebug()->AddLine("Animations loaded");
        } else {
            CleoFunctions::LOAD_ANIMATION("GANGS");
            CleoFunctions::LOAD_ANIMATION("POLICE");
            CleoFunctions::LOAD_ANIMATION("MEDIC");
        }
    }

    Peds::Update();
    Vehicles::Update();
    Pullover::Update();

    CleoFunctions::Update(menuSZK->deltaTime);

    logInternal("PoliceMod: Update end");
}

void PoliceMod::TestEquip()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 31, 800);
    CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 22, 200);
    // if(!Ped::PedHasWeaponId(playerActor, 10))
    // {
    //     CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
    // }
    CleoFunctions::CHANGE_PLAYER_MODEL_TO(0, 280);
}