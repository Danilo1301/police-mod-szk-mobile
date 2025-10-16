#include "Pullover.h"
#include "Peds.h"
#include "CleoFunctions.h"
#include "Criminals.h"

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
    menuDebug->AddLine("~r~nobody to pullover");
}

void Pullover::TryPulloverOnFoot()
{
    fileLog->Log("try pullover on foot");

    aimingPed = FindAimingPed();

    if(aimingPed == NO_PED_FOUND)
    {
        menuDebug->AddLine("~r~no ped found");
        return;
    }

    auto ped = Peds::GetPed(aimingPed);

    PulloverPed(ped);
}

void Pullover::PulloverPed(Ped* ped)
{
    menuDebug->AddLine("~g~pull over ped");

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
    auto window = menuSZK->CreateWindow(200, 200, 800, "Abordagem");

    {
        auto button = window->AddButton("~r~Liberar");
        button->onClick->Add([window, ped]() {
            window->Close();

            FreePed(ped);
        });
    }

    {
        auto button = window->AddButton("~r~Fechar");
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}