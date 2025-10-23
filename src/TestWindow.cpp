#include "TestWindow.h"

#include "CleoFunctions.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "BottomMessage.h"
#include "Chase.h"
#include "Callouts.h"

void TestWindow::OpenWindow()
{
    auto window = menuSZK->CreateWindow(200, 200, 800, "Mod Policia - tests");

    {
        auto button = window->AddButton("Equip");
        button->onClick->Add([window]() {
            window->Close();

            SET_MAX_WANTED_LEVEL_TO(0);
            SET_PLAYER_WANTED_LEVEL(0, 0);
            
            // if(!Ped::PedHasWeaponId(playerActor, 10))
            // {
            //     CleoFunctions::GIVE_ACTOR_WEAPON(playerActor, 10, 1);
            // }
            
            ModelLoader::AddModelToLoad(356);
            ModelLoader::AddModelToLoad(346);
            ModelLoader::AddModelToLoad(280);
            ModelLoader::LoadAll([]() {
                int playerActor = GET_PLAYER_ACTOR(0);

                GIVE_ACTOR_WEAPON(playerActor, 31, 800);
                GIVE_ACTOR_WEAPON(playerActor, 22, 200);
                CHANGE_PLAYER_MODEL_TO(0, 280);
            });
        });
    }

    {
        auto button = window->AddButton("Freeze criminals");
        button->onClick->Add([window]() {
            window->Close();

            auto vehicles = Chase::vehiclesInChase;

            for(auto vehicle : vehicles)
            {
                SET_CAR_MAX_SPEED(vehicle->ref, 0);
            }

            BottomMessage::SetMessage("Velocity limited to 0", 2000);
        });
    }

    {
        auto button = window->AddButton("Spawn dangerous ped");
        button->onClick->Add([window]() {
            window->Close();
            
            BottomMessage::SetMessage("Spawnando pedestre que reage a abordagem", 3000);

            ModelLoader::AddModelToLoad(80);
            ModelLoader::LoadAll([]() {

                auto playerPosition = GetPlayerPosition();
                auto pedPath = STORE_PED_PATH_COORDS_CLOSEST_TO(playerPosition.x, playerPosition.y, playerPosition.z);

                auto pedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, 80, pedPath.x, pedPath.y, pedPath.z);
                auto ped = Peds::RegisterPed(pedRef);
                ped->flags.willSurrender = false;

                ped->ShowBlip(CRGBA(0, 255, 255));
            });
        });
    }

    {
        auto button = window->AddButton("Accept callout");
        button->onClick->Add([window]() {
            window->Close();
            
            Callouts::AcceptCallout();
        });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}