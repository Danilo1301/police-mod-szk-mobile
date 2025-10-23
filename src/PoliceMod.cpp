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
        Peds::RemovePed(ref);
    });

    menuSZK->onVehicleAdded->Add([](int ref) {
        auto ptr = menuSZK->GetCVehicleFromRef(ref);
        Vehicles::AddVehicle(ref, ptr);
    });
    
    menuSZK->onVehicleRemoved->Add([](int ref) {
        Vehicles::RemoveVehicle(ref);
    });

    menuSZK->onGameUpdate->Add([this]() {
        OnGameUpdate();
    });

    textureBlip = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/blip.png"));
    textureCircle = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/map/circle.png"));
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
    });
}

void PoliceMod::OnGameUpdate()
{
    fileLog->debugEnabled = menuSZK->debug->enabled;
    
    auto ppos = GetPlayerPosition();

    g_playerPosition = new CVector(ppos);

    if(!hasFirstUpdated)
    {
        hasFirstUpdated = true;
        OnFirstUpdate();
    }

    fileLog->Debug("Update systems...");
    Criminals::Update();
    Peds::Update();
    Vehicles::Update();
    BottomMessage::Update();
    TopMessage::Update();
    Chase::Update();
    AIController::Update();
    Escort::Update();
    PoliceBases::Update();
    Checkpoints::Update();
    Callouts::Update();
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