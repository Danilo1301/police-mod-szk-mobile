#include "PoliceMod.h"

#include "globals.h"
#include "Pullover.h"
#include "RadioWindow.h"
#include "Peds.h"
#include "CleoFunctions.h"
#include "Criminals.h"
#include "TestWindow.h"
#include "BottomMessage.h"

bool hasFirstUpdated = false;

PoliceMod::PoliceMod()
{

}

void PoliceMod::OnModPreLoad()
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

    menuSZK->onGameUpdate->Add([this]() {
        OnGameUpdate();
    });

    textureBlip = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/blip.png"));
    textureCircle = menuSZK->LoadTexture(modData->GetFileFromMenuSZK("assets/map/circle.png"));

    menuSZK->onDrawBeforeMenu->Add([]() {
        auto peds = Peds::GetPedsMap();

        for(auto pair : peds)
        {
            auto ped = pair.second;

            if(ped->flags.showBlip == false) continue;

            if(!Peds::IsValid(ped)) continue;

            auto position = ped->GetPosition();
            position.z += 1.8f;
        
            menuSZK->DrawTextureOnWorld(textureBlip, position, ped->flags.blipColor, CVector2D(100, 100));
        }

        BottomMessage::Draw();
    });

    menuSZK->onBeforeMenuUpdate->Add([]() {
        UpdateWorldWidgets();
    });

    menuSZK->onPostDrawRadar->Add([]() {
        auto peds = Peds::GetPedsMap();

        for(auto pair : peds)
        {
            auto ped = pair.second;

            if(ped->flags.showBlip == false) continue;

            if(!Peds::IsValid(ped)) continue;

            auto position = ped->GetPosition();
        
            menuSZK->DrawTextureOnRadar(textureCircle, position, ped->flags.blipColor, 16.0f);
        }
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

    Criminals::Update();
    Peds::Update();
    //Vehicles::Update();
    CleoFunctions::Update(menuSZK->deltaTime);
}

void PoliceMod::OnFirstUpdate()
{
    menuDebug->AddLine("mod polica: first update");

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
        auto widget = menuSZK->CreateWidget(
            500 + 170 + 170,
            30,
            150,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFileFromMenuSZK("assets/widget_menu.png")
        );

        widget->onClick->Add([]() {
            TestWindow::OpenWindow();
        });
    }
}

PoliceMod* policeMod = new PoliceMod();