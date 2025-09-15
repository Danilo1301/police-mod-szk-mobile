#include "Pullover.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"

int aimingPed = NO_PED_FOUND;
std::vector<Ped*> pedsPulledOver;
std::vector<Vehicle*> vehiclesPulledOver;

bool isMenuOpen = false;

void Pullover::Initialize()
{
    {
        auto widget = menuSZK->CreateWidgetButton(350, 350, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_pullover.png"));
        widget->onClickWidget->Add([]() {

            int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

            if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor))
            {
                TryPulloverFromVehicle();
                return;
            }

            if(aimingPed == NO_PED_FOUND)
            {
                TryPulloverVehicle();
                return;
            }

            auto ped = Peds::GetPed(aimingPed);

            if(IsPedBeeingPulledOver(ped)) return;

            PulloverPed(ped);
        });
    }
}

void Pullover::Update()
{
    aimingPed = GetCharPlayerIsAiming();

    if(aimingPed == NO_PED_FOUND)
    {
        //menuSZK->GetDebug()->AddLine("pullover update");
    } else {
        //menuSZK->GetDebug()->AddLine("AIMING at someone!");
    }
}

int Pullover::GetCharPlayerIsAiming()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    for(int ped = 0; ped < 35584; ped++)
    {
        if(CleoFunctions::PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            return ped;
        }
    }
    return NO_PED_FOUND;
}

void Pullover::PulloverPed(Ped* ped)
{
    menuSZK->GetDebug()->AddLine("Pulling over a ped");
    
    pedsPulledOver.push_back(ped);
    ped->DoHandsup();
    ped->AddBlip();
}

void Pullover::FreePed(Ped* ped)
{
    menuSZK->GetDebug()->AddLine("~g~free ped");

    pedsPulledOver.erase(std::find(pedsPulledOver.begin(), pedsPulledOver.end(), ped));
    ped->StopHandsup();
}

void Pullover::FreeVehicle(Vehicle* vehicle)
{
    menuSZK->GetDebug()->AddLine("~g~free vehicle");

    vehiclesPulledOver.erase(std::find(vehiclesPulledOver.begin(), vehiclesPulledOver.end(), vehicle));
    
    vehicle->RemoveBlip();

    auto owners = vehicle->GetOwners();
    for(auto owner : owners)
    {
        owner->RemoveBlip();
    }

    auto ownersCount = vehicle->GetOwners().size();
    
    vehicle->MakeOwnersEnter();

    CleoFunctions::AddWaitForFunction([vehicle, ownersCount] () {
        auto ocuppantsCount = vehicle->GetCurrentOccupants().size();

        debug->AddLine("esperando passageiros " + std::to_string(ocuppantsCount) + "/" + std::to_string(ownersCount));

        if(ocuppantsCount >= ownersCount)
        {
            return true;
        }

        return false;
    }, [vehicle] () {
        auto driver = vehicle->GetCurrentDriver();
        
        if(driver) driver->StartDrivingRandomly();
    });
}

void Pullover::TryPulloverFromVehicle()
{
    TryPulloverVehicle();
}

void Pullover::TryPulloverVehicle()
{
    int playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if(CleoFunctions::IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    CleoFunctions::STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    Vehicle* closestCar = Vehicles::GetClosestVehicle(CVector(x, y, z), 5.0f);

    if(closestCar == NULL)
    {
        debug->AddLine("~r~no car found to pull over");
        return;
    }
    
    if(IsVehicleBeeingPulledOver(closestCar))
    {
        debug->AddLine("~r~already beeing pulled over");
        return;
    }

    PulloverVehicle(closestCar);
}

void Pullover::PulloverVehicle(Vehicle* vehicle)
{
    debug->AddLine("~g~pull over vehicle");

    vehiclesPulledOver.push_back(vehicle);

    vehicle->AddBlip();
    vehicle->SetOwners();

    CleoFunctions::WAIT(1000, [vehicle]() {
        CleoFunctions::CAR_TURN_OFF_ENGINE(vehicle->ref);
    });
}

bool Pullover::IsPedBeeingPulledOver(Ped* ped)
{
    auto it = std::find(pedsPulledOver.begin(), pedsPulledOver.end(), ped);
    return it != pedsPulledOver.end();
}

bool Pullover::IsVehicleBeeingPulledOver(Vehicle* vehicle)
{
    auto it = std::find(vehiclesPulledOver.begin(), vehiclesPulledOver.end(), vehicle);
    return it != vehiclesPulledOver.end();
}

void Pullover::OpenPedMenu(Ped* ped)
{
    isMenuOpen = true;

    auto window = menuSZK->CreateWindow(400, 200, 800, "Abordagem");
    
    {
        window->AddText("Pedestre " + std::to_string(ped->ref));
    }

    if(ped->pulledOverFromVehicle == 0)
    {
        auto button = window->AddButton("> ~r~Liberar");
        button->onClick->Add([window, ped](IContainer*) {
            FreePed(ped);
            window->Close();
            isMenuOpen = false;
        });
    } else {
        auto fromVehicle = Vehicles::GetVehicle(ped->pulledOverFromVehicle);
        if(fromVehicle != NULL)
        {
            auto button = window->AddButton("> ~r~Liberar veiculo");
            button->onClick->Add([window, fromVehicle](IContainer*) {
                FreeVehicle(fromVehicle);
                window->Close();
                isMenuOpen = false;
            });
        }
    }

    {
        auto button = window->AddButton("> ~y~Pedir para aguardar");
        button->onClick->Add([window](IContainer*) {
            window->Close();
            isMenuOpen = false;
        });
    }
}

void Pullover::OpenVehicleMenu(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(400, 200, 800, "Abordagem");
    
    {
        window->AddText("Veiculo " + std::to_string(vehicle->ref));
    }

    auto numOcuppants = vehicle->GetCurrentOccupants().size();
    if(numOcuppants > 0)
    {
        auto button = window->AddButton("> Descer com as maos na cabeca");
        button->onClick->Add([window, vehicle](IContainer*) {

            auto peds = vehicle->GetCurrentOccupants();

            for(auto ped : peds)
            {
                ped->LeaveCar();
                ped->pulledOverFromVehicle = vehicle->ref;

                if(!IsPedBeeingPulledOver(ped))
                {
                    PulloverPed(ped);
                }
            }

            window->Close();
            isMenuOpen = false;
        });
    }

    {
        auto button = window->AddButton("> ~r~Liberar veiculo");
        button->onClick->Add([window, vehicle](IContainer*) {
            FreeVehicle(vehicle);
            window->Close();
            isMenuOpen = false;
        });
    }

    {
        auto button = window->AddButton("> ~y~Pedir para aguardar");
        button->onClick->Add([window](IContainer*) {
            window->Close();
            isMenuOpen = false;
        });
    }
}

bool Pullover::IsPulloverMenuOpen()
{
    return isMenuOpen;
}