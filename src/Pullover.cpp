#include "Pullover.h"

#include "Peds.h"
#include "CleoFunctions.h"
#include "Criminals.h"
#include "ModelLoader.h"
#include "Vehicles.h"
#include "BottomMessage.h"

int aimingPed = NO_PED_FOUND;

void Pullover::OnClickWidget()
{
    fileLog->Log("Clicked pullover widget");

    int playerActor = GET_PLAYER_ACTOR(0);

    if(IS_CHAR_IN_ANY_CAR(playerActor))
    {
        TryPulloverFromVehicle();
    } else {
        TryPulloverOnFoot();
    }

    blockInput = !blockInput;
}

void Pullover::TryPulloverFromVehicle()
{
    int playerActor = GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if(IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    Vehicle* closestCar = Vehicles::GetClosestVehicleNotPlayer(CVector(x, y, z), GetPlayerPosition(), 5.0f);

    if(closestCar == NULL)
    {
        BottomMessage::SetMessage(GetTranslatedText("pullover_from_vehicle_fail"), 3000);
        return;
    }

    PulloverVehicle(closestCar);
}

void Pullover::TryPulloverOnFoot()
{
    fileLog->Log("try pullover on foot");

    aimingPed = FindAimingPed();

    if(aimingPed == NO_PED_FOUND)
    {
        menuDebug->AddLine("~r~no ped found");
        BottomMessage::SetMessage(GetTranslatedText("pullover_on_foot_fail"), 3000);
        return;
    }

    auto ped = Peds::GetPed(aimingPed);

    PulloverPed(ped);
}

void Pullover::PulloverPed(Ped* ped)
{
    menuDebug->AddLine("~g~pull over ped");

    if(ped->flags.isInconcious) return;

    if(ped->flags.willSurrender == false)
    {
        ped->ShowBlip(COLOR_CRIMINAL);
        Criminals::AddCriminal(ped);

        ModelLoader::AddModelToLoad(346);
        ModelLoader::LoadAll([ped]() {
            GIVE_ACTOR_WEAPON(ped->ref, 22, 500);
        });

        KILL_ACTOR(ped->ref, GetPlayerActor());

        return;
    }

    ped->flags.hasSurrended = true;
    ped->SetCanDoHandsup(true);
    ped->ShowBlip(COLOR_CRIMINAL);
    Criminals::AddCriminal(ped);
}

void Pullover::FreePed(Ped* ped)
{
    menuDebug->AddLine("~g~free ped");

    ped->flags.hasSurrended = false;
    ped->SetCanDoHandsup(false);
    ped->HideBlip();
    Criminals::RemoveCriminal(ped);
}

void Pullover::PulloverVehicle(Vehicle* vehicle)
{
    vehicle->ShowBlip(COLOR_CRIMINAL);

    WAIT(1000, [vehicle]() {
        CAR_TURN_OFF_ENGINE(vehicle->ref);
    });

    WAIT(2500, []() {
        BottomMessage::SetMessage(GetTranslatedText("pullover_get_close_to_vehicle"), 3000);
    });
}

int Pullover::FindAimingPed()
{
    for(int ped = 1; ped < 35584; ped++)
    {
        if(PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            aimingPed = ped;
            return ped;
        }
    }
    return NO_PED_FOUND;
}

void Pullover::OpenPedMenu(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, "Abordagem");
    
    if(ped->flags.isInconcious == false)
    {
        auto button = window->AddButton("~r~Liberar");
        button->onClick->Add([window, ped]() {
            window->Close();

            FreePed(ped);
        });
    }

    {
        auto button = window->AddButton("~y~Fechar");
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}