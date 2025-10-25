#include "PoliceMod.h"

#include "Pullover.h"
#include "RadioWindow.h"
#include "Peds.h"
#include "Vehicles.h"
#include "CleoFunctions.h"
#include "Criminals.h"
#include "TestWindow.h"
#include "BottomMessage.h"
#include "TopMessage.h"
#include "WorldWidgets.h"
#include "Chase.h"
#include "AIController.h"
#include "Escort.h"
#include "PoliceBases.h"
#include "ATMSystem.h"
#include "Callouts.h"
#include "BackupUnits.h"

bool hasFirstUpdated = false;

PoliceMod::PoliceMod()
{

}

void PoliceMod::OnModPreLoad()
{

}

void PoliceMod::OnModLoad()
{
    menuSZK->RegisterLocalizationFolder(menuSZK->GetLocalizationsPath() + "/policeModSZK/");

    menuSZK->onPedAdded->Add([](int ref) {
        auto ptr = menuSZK->GetCPedFromRef(ref);
        Peds::AddPed(ref, ptr);
    });
    
    menuSZK->onPedRemoved->Add([](int ref) {
        fileLog->Log("onPedRemoved()");

        Peds::RemovePed(ref);
    });

    menuSZK->onVehicleAdded->Add([](int ref) {
        auto ptr = menuSZK->GetCVehicleFromRef(ref);
        Vehicles::AddVehicle(ref, ptr);
    });
    
    menuSZK->onVehicleRemoved->Add([](int ref) {
        fileLog->Log("onVehicleRemoved()");
    
        g_onVehicleDestroy->Emit(ref);

        Vehicles::RemoveVehicle(ref);
    });

    menuSZK->onGameUpdate->Add([this]() {
        OnGameUpdate();
    });

    textureBlip = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/blip.png"));
    textureCircle = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/map/circle.png"));
    textureBigCircle = menuSZK->LoadTexture(modData->GetFileFromAssets("map/big_circle.png"));
    texturePoliceDP = menuSZK->LoadTexture(modData->GetFileFromAssets("map/police_dep.png"));

    menuSZK->onDrawBeforeMenu->Add([]() {
        auto peds = Peds::GetPedsMap();
        for(auto pair : peds)
        {
            auto ped = pair.second;

            if(!Peds::IsValid(ped)) continue;

            if(ped->flags.showBlip == false) continue;
            if(ped->IsInAnyCar()) continue;

            auto position = ped->GetPosition();
            position.z += 1.8f;
        
            menuSZK->DrawTextureOnWorld(textureBlip, position, ped->flags.blipColor, CVector2D(100, 100));
        }

        auto vehicles = Vehicles::GetVehiclesMap();
        for(auto pair : vehicles)
        {
            auto vehicle = pair.second;

            if(!Vehicles::IsValid(vehicle)) continue;
            
            if(vehicle->flags.showBlip == false) continue;

            auto position = vehicle->GetPosition();
            position.z += 2.2f;
        
            menuSZK->DrawTextureOnWorld(textureBlip, position, vehicle->GetBlipColor(), CVector2D(100, 100));
        }

        PoliceBases::OnDraw();
    });

    menuSZK->onBeforeMenuUpdate->Add([]() {
        UpdateWorldWidgets();
    });

    menuSZK->onPostDrawRadar->Add([]() {
        auto peds = Peds::GetPedsMap();
        for(auto pair : peds)
        {
            auto ped = pair.second;

            if(!Peds::IsValid(ped)) continue;

            if(ped->flags.showBlip == false) continue;
            if(ped->IsInAnyCar()) continue;

            auto position = ped->GetPosition();
        
            menuSZK->DrawTextureOnRadar(textureCircle, position, ped->flags.blipColor, 14.0f);
        }

        auto vehicles = Vehicles::GetVehiclesMap();
        for(auto pair : vehicles)
        {
            auto vehicle = pair.second;

            if(!Vehicles::IsValid(vehicle)) continue;

            if(vehicle->flags.showBlip == false) continue;

            auto position = vehicle->GetPosition();
        
            menuSZK->DrawTextureOnRadar(textureCircle, position, vehicle->GetBlipColor(), 20.0f);
        }

        PoliceBases::OnPostDrawRadar();
        BackupUnits::OnPostDrawRadar();
    });
}

void PoliceMod::OnGameUpdate()
{
    fileLog->debugEnabled = menuSZK->debug->enabled;
    
    // check

    BackupUnits::CheckIfVehiclesAreValid();
    Criminals::CheckIfCriminalsAreValid();

    //

    auto ppos = GetPlayerPosition();
    g_playerPosition = new CVector(ppos);

    if(!hasFirstUpdated)
    {
        hasFirstUpdated = true;
        OnFirstUpdate();
    }

    fileLog->Debug("Update systems [1]");

    if(menuSZK->debug->enabled)
    {
        if(PLAYER_DEFINED(0))
        {
            auto playerActor = GetPlayerActor();

            if(ACTOR_HEALTH(playerActor) < 20)
            {
                SET_ACTOR_HEALTH(playerActor, 100);
            }
        }
    }
    
    Criminals::Update();
    Peds::Update();
    Vehicles::Update();
    fileLog->Debug("Update systems [2]");
    BottomMessage::Update();
    TopMessage::Update();
    fileLog->Debug("Update systems [3]");
    Chase::Update();
    BackupUnits::Update();
    fileLog->Debug("Update systems [4]");
    AIController::Update();
    Escort::Update();
    fileLog->Debug("Update systems [5]");
    PoliceBases::Update();
    Checkpoints::Update();
    fileLog->Debug("Update systems [6]");
    Callouts::Update();
    fileLog->Debug("Update systems [7]");

    for(auto pedRef : g_pedsToDestroy)
    {
        if(!ACTOR_DEFINED(pedRef)) continue;

        DESTROY_ACTOR(pedRef);
        Peds::RemovePed(pedRef);
    }
    g_pedsToDestroy.clear();

    for(auto carRef : g_vehiclesToDestroy)
    {
        if(!CAR_DEFINED(carRef)) continue;

        DESTROY_CAR(carRef);
        Vehicles::RemoveVehicle(carRef);
    }
    g_vehiclesToDestroy.clear();

    fileLog->Debug("Finished updating systems");
    CleoFunctions::Update(menuSZK->deltaTime);
    fileLog->Debug("Finished updating cleo functions");
}

void PoliceMod::OnFirstUpdate()
{
    menuDebug->AddLine("mod polica: first update");

    LOAD_ANIMATION("GANGS");
    LOAD_ANIMATION("POLICE");
    LOAD_ANIMATION("MEDIC");
    LOAD_ANIMATION("CRACK");

    BottomMessage::Initialize();
    TopMessage::Initialize();
    RadioWindow::Initialize();
    PoliceBases::Initialize();
    BackupUnits::Initialize();
    ATMSystem::Initialize();

    {
        auto widget = menuSZK->CreateWidget(
            500,
            30,
            150,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );

        widget->onClick->Add([]() {
            Pullover::OnClickWidget();
        });
    }

    {
        auto widget = menuSZK->CreateWidget(
            500 + 170,
            30,
            150,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_radio.png")
        );

        widget->onClick->Add([]() {
            RadioWindow::Toggle();
        });
    }

    {
        // auto widget = menuSZK->CreateWidget(
        //     500 + 170 + 170,
        //     30,
        //     150,
        //     modData->GetFileFromMenuSZK("assets/widget_background1.png"),
        //     modData->GetFileFromMenuSZK("assets/widget_menu.png")
        // );

        // widget->onClick->Add([]() {
        //     TestWindow::OpenWindow();
        // });
    }
}

PoliceMod* policeMod = new PoliceMod();