#include "Pullover.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"
#include "windows/RGWindow.h"
#include "Audios.h"
#include "PoliceMod.h"

int aimingPed = NO_PED_FOUND;
std::vector<Ped*> pedsPulledOver;
std::vector<Vehicle*> vehiclesPulledOver;

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
    ped->RemoveBlip();
}

void Pullover::FreeVehicle(Vehicle* vehicle)
{
    menuSZK->GetDebug()->AddLine("~g~free vehicle");

    vehiclesPulledOver.erase(std::find(vehiclesPulledOver.begin(), vehiclesPulledOver.end(), vehicle));
    
    vehicle->RemoveBlip();

    Audios::audioLiberado->Play();

    auto owners = vehicle->GetOwners();
    for(auto owner : owners)
    {
        owner->RemoveBlip();
    }

    auto ownersCount = vehicle->GetOwners().size();
    
    vehicle->MakeOwnersEnter();

    auto ocuppantsCount = vehicle->GetCurrentOccupants().size();
    debug->AddLine("esperando passageiros " + std::to_string(ocuppantsCount) + "/" + std::to_string(ownersCount));

    CleoFunctions::AddWaitForFunction([vehicle, ownersCount] () {

        if(!Vehicles::IsValid(vehicle))
        {
            return true;
        }

        auto ocuppantsCount = vehicle->GetCurrentOccupants().size();

        if(ocuppantsCount >= ownersCount)
        {
            return true;
        }

        return false;
    }, [vehicle] () {

        if(!Vehicles::IsValid(vehicle)) return;

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

    Audios::audioEncostaCarro->Play();

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
    auto window = menuSZK->CreateWindow(400, 200, 800, "Abordagem");
    
    PoliceMod::m_IsUsingMenu = true;

    auto closeWindow = [window]() {
        window->Close();
        PoliceMod::m_IsUsingMenu = false;
    };

    {
        window->AddText("Pedestre " + std::to_string(ped->ref));
    }

    {
        auto button = window->AddButton("> Pedir RG");
        button->onClick->Add([window, ped, closeWindow](IContainer*) {
            closeWindow();

            Audios::audioPedirRG->Play();

            CleoFunctions::AddWaitForFunction([]() {

                if(Audios::audioPedirRG->HasEnded())
                {
                    return true;
                }

                return false;
            }, [ped]() {
                RGWindow::CreateRG(ped);
            });
        });
    }

    if(ped->pulledOverFromVehicle == 0)
    {
        auto button = window->AddButton("> ~r~Liberar");
        button->onClick->Add([ped, closeWindow](IContainer*) {
            closeWindow();

            FreePed(ped);
        });
    } else {
        auto fromVehicle = Vehicles::GetVehicle(ped->pulledOverFromVehicle);
        if(fromVehicle != NULL)
        {
            auto button = window->AddButton("> ~r~Liberar veiculo");
            button->onClick->Add([closeWindow, fromVehicle](IContainer*) {
                closeWindow();

                FreeVehicle(fromVehicle);
            });
        }
    }

    {
        auto button = window->AddButton("> ~y~Pedir para aguardar");
        button->onClick->Add([closeWindow](IContainer*) {
            closeWindow();
        });
    }
}

void Pullover::OpenVehicleMenu(Vehicle* vehicle)
{
    PoliceMod::m_IsUsingMenu = true;


    
    auto window = menuSZK->CreateWindow(400, 200, 800, "Abordagem");

    auto closeWindow = [window]() {
        window->Close();
        PoliceMod::m_IsUsingMenu = false;
    };
    
    {
        window->AddText("Veiculo " + std::to_string(vehicle->ref));
    }

    auto numOcuppants = vehicle->GetCurrentOccupants().size();
    if(numOcuppants > 0)
    {
        auto button = window->AddButton("> Descer com as maos na cabeca");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {

            closeWindow();

            Audios::audioDesceMaoCabeca->Play();

            CleoFunctions::WAIT(3000, [vehicle]() {
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
            });
        });
    }

    auto button = window->AddButton("> Consultar placa ~b~" + vehicle->plate);
    button->onClick->Add([closeWindow, vehicle](IContainer*) {
        closeWindow();

        auto audioGroup = currentAudioGroup = new AudioGroup();
        audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets("audios/ht.wav")));
        audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets("audios/officer/license_plate_check.wav")));

        auto plate = vehicle->plate;

        debug->AddLine("Consulta de placa: ~y~" + plate);

        // percorrendo cada caractere
        for (size_t i = 0; i < plate.size(); ++i)
        {
            char c = plate[i];

            if (c == ' ') continue; // pula espaços

            // monta o caminho do áudio dinamicamente
            std::string audioPath = "audios/officer/codes/";
            audioPath += c;        // adiciona a letra ou número
            audioPath += ".wav";   // extensão

            audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets(audioPath)));
        }

        audioGroup->Play();
    });

    {
        auto button = window->AddButton("> ~r~Liberar veiculo");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {
            closeWindow();

            FreeVehicle(vehicle);
        });
    }

    {
        auto button = window->AddButton("> ~y~Pedir para aguardar");
        button->onClick->Add([closeWindow](IContainer*) {
            closeWindow();
        });
    }
}