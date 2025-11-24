#include "Escort.h"

#include "CleoFunctions.h"
#include "TopMessage.h"
#include "Vehicles.h"
#include "Peds.h"
#include "ScriptTask.h"
#include "BottomMessage.h"

Ped* g_escortingPed = nullptr;
Ped* g_carryingPed = nullptr;

bool g_pedWasInAVehicle = false;

void Escort::Update()
{
    bool playerActor = GetPlayerActor();
    bool isInCar = IS_CHAR_IN_ANY_CAR(playerActor);

    if(g_pedWasInAVehicle != isInCar)
    {
        g_pedWasInAVehicle = isInCar;

        if(isInCar)
        {
            OnPlayerEnterVehicle();
        }
    }

    if(g_carryingPed)
    {
        auto playerPosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 1.1f, 0));
        
        g_carryingPed->SetPosition(playerPosition);
    }
}

void Escort::OpenEscortWindow(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_escort"));
    
    {
        auto button = window->AddButton(GetTranslatedText("escort_carry"));
        button->onClick->Add([window, ped]() {
            window->Close();

            if(g_carryingPed)
            {
                BottomMessage::SetMessage(GetTranslatedText("error_already_escorting"), 3000);
                return;
            }

            g_carryingPed = ped;

            BottomMessage::SetMessage("Carregando o suspeito", 3000);
        });
    }

    {
        auto button = window->AddButton(GetTranslatedText("escort_follow"));
        button->onClick->Add([window, ped]() {
            window->Close();

            if(Escort::IsEscortingSomeone())
            {
                BottomMessage::SetMessage(GetTranslatedText("error_already_escorting"), 3000);
                return;
            }

            ped->flags.showWidget = false;

            Escort::EscortPed(ped);
        });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void Escort::OpenCarryingPedOptions(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_escort"));
    
    {
        auto button = window->AddButton(GetTranslatedText("stop_carry"));
        button->onClick->Add([window]() {
            window->Close();

            g_carryingPed = nullptr;
        });
    }

    {
        auto button = window->AddButton(GetTranslatedText("put_ped_in_trunk"));
        button->onClick->Add([window]() {
            window->Close();

            auto vehicle = GetClosestOpenTrunk();

            if(!vehicle)
            {
                BottomMessage::SetMessage("~r~O suspeito nao esta perto de um porta malas", 3000);
                return;
            }

            vehicle->trunk->AddPedToTrunk(g_carryingPed->ref);

            g_carryingPed = nullptr;
        });
    }

    {
        auto button = window->AddButton("~r~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void Escort::EscortPed(Ped* _ped)
{
    auto oldPed = _ped;

    //

    auto modelId = GET_ACTOR_MODEL(oldPed->ref);
    auto position = oldPed->GetPosition();

    auto newPedRef = CREATE_ACTOR_PEDTYPE(PedType::Special, modelId, position.x, position.y, position.z - 0.8f);
    auto newPed = Peds::RegisterPed(newPedRef);

    newPed->CopyFrom(*oldPed);
    newPed->ClearAnim();
    REMOVE_REFERENCES_TO_ACTOR(newPed->ref);

    oldPed->DestroyImmediate();
    
    //

    g_escortingPed = newPed;

    TopMessage::SetMessage(GetTranslatedText("escort_to_cop_vehicle"));
    
    //

    ScriptTask* taskEnter = new ScriptTask("enter");
    taskEnter->SetStartAgainIfTimeout(3000);
    taskEnter->onBegin = [newPed]() {
        if(newPed->isEnteringCar) return;

        //BottomMessage::SetMessage("make it follow", 1000);

        CLEAR_ACTOR_TASK(newPed->ref);
        TASK_FOLLOW_FOOTSTEPS(newPed->ref, GetPlayerActor());
    };
    taskEnter->onExecute = [newPed, taskEnter]() {        
        if(!Peds::IsValid(newPed)) return SCRIPT_CANCEL;

        if(newPed->IsInAnyCar()) return SCRIPT_SUCCESS;

        if(taskEnter->totalTimeElapsed > 70000)
        {
            return SCRIPT_CANCEL;
        }

        return SCRIPT_KEEP_GOING;
    };
    taskEnter->onCancel = []() {
        TopMessage::ClearMessage();
    };
    taskEnter->onComplete = []() {
    };
    taskEnter->Start();
}

void Escort::OnPlayerEnterVehicle()
{
    auto vehicleRef = GetVehiclePedIsUsing(GetPlayerActor());
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    if(!vehicle) return;
    if(!g_escortingPed) return;

    TopMessage::ClearMessage();

    auto ped = g_escortingPed;

    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_escort_to_seat"));

    {
        auto button = window->AddButton("Banco de tras");
        button->onClick->Add([window, vehicle, ped]() {
            window->Close();

            //ped->flags.showWidget = false;
            
            //   0
            // 1 2

            int maxPassengers = CAR_MAX_PASSENGERS(vehicle->ref);
            int lastFreeSeat = -1; // -1 = nenhum assento livre

            for(int seatID = 0; seatID <= maxPassengers; seatID++)
            {
                if(CAR_PASSENGER_SEAT_FREE(vehicle->ref, seatID))
                {
                    lastFreeSeat = seatID; // guarda o último assento livre encontrado
                }
            }

            if(lastFreeSeat == -1)
            {
                BottomMessage::SetMessage("~r~Nenhum assento livre", 3000);
                return;
            }

            ped->EnterVehicle(vehicle->ref, lastFreeSeat + 1);
            
            CleoFunctions::AddWaitForFunction("escort_wait_to_enter", [ped]() {
                if(!Peds::IsValid(ped)) return true;
                if(ped->IsInAnyCar()) return true;

                return false;
            }, [ped]() {
                SET_CHAR_STAY_IN_CAR_WHEN_JACKED(ped->ref, true);

                g_escortingPed = nullptr;
                ped->flags.beeingEscorted = true;

                TopMessage::ClearMessage();
                BottomMessage::SetMessage(GetTranslatedText("escort_to_police_base"), 5000);
            });
        });
    }
}

bool Escort::IsEscortingSomeone()
{
    return g_escortingPed != nullptr;
}

bool Escort::IsCarryingSomeone()
{
    return g_carryingPed != nullptr;
}

bool Escort::IsPedBeeingCarried(Ped* ped)
{
    return ped == g_carryingPed;
}

Vehicle* Escort::GetClosestOpenTrunk()
{
    if(!g_carryingPed) return nullptr;

    auto pedPosition = g_carryingPed->GetPosition();

    auto vehicles = Vehicles::GetAllCarsInSphere(pedPosition, 10.0f);

    for(auto vehicle : vehicles)
    {
        if(!vehicle->trunkCheckpoint) continue;

        if(vehicle->trunkCheckpoint->IsInRange(pedPosition))
        {
            return vehicle;
        }
    }

    return nullptr;
}