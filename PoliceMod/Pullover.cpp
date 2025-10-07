#include "Pullover.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"
#include "windows/RGWindow.h"
#include "windows/CNHWindow.h"
#include "windows/CRLVWindow.h"
#include "Audios.h"
#include "PoliceMod.h"
#include "ModelLoader.h"
#include "VehicleTask.h"
#include "BottomMessage.h"
#include "Objectives.h"
#include "dialog/DialogManager.h"
#include "ScriptTask.h"
#include "windows/CellphoneWindow.h"
#include "Chase.h"
#include "Criminals.h"
#include "BackupUnits.h"
#include "SpriteUtils.h"
#include "Scorch.h"

int aimingPed = NO_PED_FOUND;
std::vector<Ped*> pedsPulledOver;
std::vector<Vehicle*> vehiclesPulledOver;

void Pullover::Initialize()
{
    {
        auto widget = menuSZK->CreateWidgetButton(600, 30, getPathFromMenuAssets("widget_background1.png"), getPathFromAssets("widget_pullover.png"));
        widget->onClickWidget->Add([]() {

            int playerActor = GET_PLAYER_ACTOR(0);

            if(IS_CHAR_IN_ANY_CAR(playerActor))
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
    int playerActor = GET_PLAYER_ACTOR(0);

    for(int ped = 0; ped < 35584; ped++)
    {
        if(PLAYER_AIMING_AT_ACTOR(0, ped))
        {
            return ped;
        }
    }
    return NO_PED_FOUND;
}

void Pullover::PulloverPed(Ped* ped, bool doAction)
{
    auto it = std::find(pedsPulledOver.begin(), pedsPulledOver.end(), ped);
    if (it != pedsPulledOver.end())
        return;

    if(doAction)
    {
        menuSZK->GetDebug()->AddLine("Pulling over a ped");
    }
    
    pedsPulledOver.push_back(ped);
    ped->hasSurrended = true;
    ped->DoHandsup();
    ped->SetMapIconColor(COLOR_CRIMINAL);

    Criminals::AddCriminal(ped);
}

void Pullover::FreePed(Ped* ped)
{
    menuSZK->GetDebug()->AddLine("~g~free ped");

    RemovePedFromPullover(ped);
    
    ped->StopHandsup();
    ped->HideMapIcon();
}

void Pullover::RemovePedFromPullover(Ped* ped)
{
    pedsPulledOver.erase(std::find(pedsPulledOver.begin(), pedsPulledOver.end(), ped));
    Criminals::RemoveCriminal(ped);
}

void Pullover::FreeVehicle(Vehicle* vehicle)
{
    RemoveVehicleFromPullover(vehicle);

    Audios::audioLiberado->Play();

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

void Pullover::RemoveVehicleFromPullover(Vehicle* vehicle)
{
    menuSZK->GetDebug()->AddLine("~g~free vehicle");

    vehiclesPulledOver.erase(std::find(vehiclesPulledOver.begin(), vehiclesPulledOver.end(), vehicle));
    
    vehicle->HideMapIcon();

    auto owners = vehicle->GetOwners();
    for(auto owner : owners)
    {
        owner->HideMapIcon();
        Criminals::RemoveCriminal(owner);
    }
}

void Pullover::TryPulloverFromVehicle()
{
    TryPulloverVehicle();
}

void Pullover::TryPulloverVehicle()
{
    int playerActor = GET_PLAYER_ACTOR(0);

    float range = 4.0f;
    if(IS_CHAR_IN_ANY_CAR(playerActor)) range = 7.0f;

    float x = 0.0f, y = 0.0f, z = 0.0f;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, 0.0f, range, 0.0f, &x, &y, &z);

    Vehicle* closestCar = Vehicles::GetClosestVehicleNotPlayer(CVector(x, y, z), GetPlayerPosition(), 5.0f);

    if(closestCar == NULL)
    {
        debug->AddLine("~r~no car found to pull over");
        BottomMessage::SetMessage("~r~Nenhum veiculo ou pedestre encontrado", 2000);
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

    vehicle->SetMapIconColor(COLOR_CRIMINAL);
    vehicle->SetOwners();

    Audios::audioEncostaCarro->Play();

    BottomMessage::AddOfficerDialog("Encosta o carro", 2000);

    if(vehicle->GetCurrentDriver() != nullptr)
    {
        bool startChase = calculateProbability(0.50);
    
        if(startChase)
        {
            Chase::StartChaseWithVehicle(vehicle);
            return;
        }
    }

    WAIT(1000, [vehicle]() {
        CAR_TURN_OFF_ENGINE(vehicle->ref);
    });

    WAIT(2500, [vehicle]() {
        BottomMessage::SetMessage("~y~Aproxime-se do veiculo!", 3000);
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
    Vehicle* vehiclePulled = Vehicles::GetVehicle(ped->pulledOverFromVehicle);
    Vehicle* vehicleOwned = NULL;
    
    if(vehiclePulled)
        vehicleOwned = vehiclePulled->hDriver == ped->ref ? vehiclePulled : NULL;

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
        auto button = window->AddButton("> Dialogos");
        button->onClick->Add([ped, closeWindow](IContainer*) {
            closeWindow();

            DialogManager::BeginDialogue(&ped->dialogue);
        });
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
                auto docWindow = RGWindow::CreateRG(ped);
                docWindow->onClose = [ped]() {
                    //OpenPedMenu(ped);
                };
            });
        });
    }

    {
        auto button = window->AddButton("> Pedir CNH");
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

                if(!ped->HasCNH())
                {
                    BottomMessage::AddMessage("~y~[Suspeito] ", "Eu nao tenho habilitacao...", 3000);
                    return;
                }

                auto docWindow = CNHWindow::CreateCNH(ped);
                docWindow->onClose = [ped]() {
                    //OpenPedMenu(ped);
                    
                };
            });
        });
    }

    if(vehicleOwned)
    {
        auto button = window->AddButton("> Pedir documento do veiculo");
        button->onClick->Add([ped, closeWindow, vehicleOwned](IContainer*) {
            closeWindow();

            if(!ped->HasCNH())
            {
                BottomMessage::AddMessage("~y~[Suspeito] ", "O veiculo nao e meu.. E nao tenho o documento", 3000);
                return;
            }

            auto docWindow = CRLV_Window::Create(ped, vehicleOwned);
            docWindow->onClose = [ped]() {
                //OpenPedMenu(ped);
            };
        });
    }

    {
        auto button = window->AddButton("> Conduzir");
        button->onClick->Add([ped, closeWindow](IContainer*) {
            closeWindow();

            ped->StopHandsup();
            Scorch::StartCarry(ped);
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

    auto numOcuppants = vehicle->GetCurrentOccupants().size();

    auto window = menuSZK->CreateWindow(400, 200, 800, "Abordagem");

    auto closeWindow = [window]() {
        window->Close();
        PoliceMod::m_IsUsingMenu = false;
    };
    
    {
        window->AddText("Veiculo " + std::to_string(vehicle->ref));
    }

   
    if(numOcuppants > 0)
    {
        auto button = window->AddButton("> Descer com as maos na cabeca");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {

            closeWindow();

            Audios::audioDesceMaoCabeca->Play();

            WAIT(2500, [vehicle]() {
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

    {
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
    }

    {
        auto button = window->AddButton("> Consultar no APP sistema");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {
            closeWindow();

            CellphoneWindow::CreateSINESPWindow(vehicle);
        });
    }

    if(numOcuppants == 0)
    {
        auto button = window->AddButton("> Pedir para habilitado buscar");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {
            closeWindow();

            vehicle->RemoveOwners();
            RemoveVehicleFromPullover(vehicle);

            BottomMessage::AddMessage("~b~[Policial] ", "Voce vai ter que chamar algum habilitado para buscar seu veiculo...", 3000);
            BottomMessage::AddMessage("~y~[Suspeito] ", "Ok, so um minuto... vou chamar aqui", 3000);

            WAIT(6000, [vehicle]() {
                Objectives::SetObjective("Aguarde o habilitado chegar com o veiculo");
            });

            WAIT(1000, [vehicle]() {
                AskSomeoneToGetVehicle(vehicle);
            });
        });
    }

    if(numOcuppants == 0)
    {
        auto button = window->AddButton("> Chamar guincho");
        button->onClick->Add([closeWindow, vehicle](IContainer*) {
            closeWindow();

            vehicle->RemoveOwners();
            RemoveVehicleFromPullover(vehicle);

            CallTowTruck(vehicle);
        });
    }

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

void Pullover::AskSomeoneToGetVehicle(Vehicle* vehicle)
{
    auto pulledVehicle = vehicle;

    int friendCarModelId = 546;
    int friendModelId = 59;

    ModelLoader::AddModelToLoad(friendCarModelId);
    ModelLoader::AddModelToLoad(friendModelId);

    ModelLoader::LoadAll([friendCarModelId, friendModelId, pulledVehicle]() {
        auto playerActor = GetPlayerActor();
        auto playerPosition = GetPedPosition(playerActor);

        auto spawnPosition = GET_CLOSEST_CAR_NODE(playerPosition.x + 120, playerPosition.y, playerPosition.z);

        auto vehicleRef = CREATE_CAR_AT(friendCarModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto vehicle = Vehicles::RegisterVehicle(vehicleRef);
        
        auto type = PedType::Special; // special

        auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicleRef, type, friendModelId);
        auto driverPed = Peds::RegisterPed(driverRef);

        auto friendRef = CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(vehicleRef, type, friendModelId, 0);
        auto pedPassenger = Peds::RegisterPed(friendRef);

        vehicle->SetOwners();
        vehicle->SetMapIconColor(COLOR_YELLOW);

        auto targetPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y, playerPosition.z);

        auto taskFollow = new VehicleTask(vehicle);
        taskFollow->DriveTo(targetPosition, [vehicle, taskFollow, pulledVehicle]() {

            delete taskFollow;

            debug->AddLine("chegou perto do player");

            Objectives::ClearObjective();

            if(!Vehicles::IsValid(vehicle)) return;

            CAR_TURN_OFF_ENGINE(vehicle->ref);

            auto passengers = vehicle->GetCurrentPassengers();
            
            if(passengers.size() == 0) return;
            
            auto passenger = passengers[0];
            
            debug->AddLine("saindo do veiculo");

            passenger->LeaveCar();

            CleoFunctions::AddWaitForFunction([passenger]() {

                if(passenger->IsInAnyCar()) return false;

                return true;
            }, [vehicle, passenger, pulledVehicle]() {
                if(!Vehicles::IsValid(vehicle)) return;

                debug->AddLine("entrando no veiculo");

                ENTER_CAR_AS_DRIVER_AS_ACTOR(passenger->ref, pulledVehicle->ref, 8000);
                
                CleoFunctions::AddWaitForFunction([passenger]() {

                    if(!passenger->IsInAnyCar()) return false;

                    return true;
                }, [vehicle, pulledVehicle]() {

                    debug->AddLine("indo embora com os carros");

                    vehicle->SetOwners();
                    vehicle->GetCurrentDriver()->StartDrivingRandomly();
                    vehicle->HideMapIcon();

                    pulledVehicle->SetOwners();
                    pulledVehicle->GetCurrentDriver()->StartDrivingRandomly();
                    pulledVehicle->HideMapIcon();
                });
            });

            // if(driverPed) driverPed->LeaveCar();
            // if(pedPassenger) pedPassenger->LeaveCar();
        });
    });
}

void Pullover::CallTowTruck(Vehicle* vehicle)
{
    int towModelId = 578;
    int driverId = 50;

    ModelLoader::AddModelToLoad(towModelId);
    ModelLoader::AddModelToLoad(driverId);

    debug->AddLine("loading models");

    ModelLoader::LoadAll([towModelId, driverId, vehicle]() {
        
        auto playerPosition = GetPlayerPosition();

        debug->AddLine("get car node");

        auto spawnPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y + 100, playerPosition.z);
        
        debug->AddLine("create car");

        auto towRef = CREATE_CAR_AT(towModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto towTruck = Vehicles::RegisterVehicle(towRef);

        towTruck->SetMapIconColor(COLOR_YELLOW);

        debug->AddLine("create driver");

        auto driver = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(towRef, PedType::CivFemale, driverId);

        debug->AddLine("get node again");

        auto targetPosition = GET_CLOSEST_CAR_NODE(playerPosition.x, playerPosition.y, playerPosition.z);

        ScriptTask* taskDrive = new ScriptTask();

        taskDrive->onBegin = [towRef, targetPosition]() {
            BottomMessage::SetMessage("Rodando task", 1000);
            
            SET_CAR_MAX_SPEED(towRef, 20.0f);
            SET_CAR_TRAFFIC_BEHAVIOUR(towRef, DrivingMode::AvoidCars);
            CAR_DRIVE_TO(towRef, targetPosition.x, targetPosition.y, targetPosition.z);
        };
        taskDrive->onExecute = [towRef, targetPosition]() {
            
            auto distance = DistanceFromVehicle(towRef, targetPosition);

            debug->AddLine("distance to " + CVectorToString(targetPosition) + " is " + std::to_string(distance));

            if(distance > 10.0f) return false;

            return true;
        };
        taskDrive->onComplete = [vehicle, towTruck]() {
            
            Objectives::ClearObjective();
            
            towTruck->HideMapIcon();

            //

            CVector offset = CVector(0, -0.5, 0.3);

            if(Vehicles::IsValid(vehicle))
            {
                ATTACH_CAR_TO_CAR(vehicle->ref, towTruck->ref, offset.x, offset.y, offset.z, 0, 0, 0);
            }

            //

            ScriptTask* taskLeave = new ScriptTask();
            taskLeave->onBegin = [towTruck]() {
                ScriptTask::MakeVehicleLeave(towTruck->ref);
            };
            taskLeave->onExecute = [towTruck]() {
                auto towPosition = GetCarPosition(towTruck->ref);
                auto distance = DistanceFromPed(GetPlayerActor(), towPosition);

                debug->AddLine("leaving, distance is " + std::to_string(distance));

                if(distance < 120.0f) return false;

                return true;
            };
            taskLeave->onComplete = [vehicle, towTruck]() {
                //on complete

                
            };
            taskLeave->Start();

            
        };
        taskDrive->Start();

        Objectives::SetObjective("Aguarde o guincho chegar ao destino");
    });
}