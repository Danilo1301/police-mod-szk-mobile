#include "Escort.h"

#include "CleoFunctions.h"
#include "TopMessage.h"
#include "Vehicles.h"
#include "Peds.h"
#include "ScriptTask.h"
#include "BottomMessage.h"

Ped* g_escortingPed = nullptr;

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
}

void Escort::EscortPed(Ped* ped)
{
    // re create

    ped->ClearAnim();

    auto oldPed = ped;
    auto modelId = GET_ACTOR_MODEL(oldPed->ref);
    auto position = ped->GetPosition();

    auto newPedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, modelId, position.x, position.y, position.z);
    ped = Peds::RegisterPed(newPedRef);

    ped->CopyFrom(*oldPed);

    DESTROY_ACTOR(oldPed->ref);
    Peds::RemovePed(oldPed->ref);
    
    //

    g_escortingPed = ped;

    TopMessage::SetMessage("~y~Leve o suspeito ate seu veiculo policial");
    
    //

    ScriptTask* taskEnter = new ScriptTask("enter");
    taskEnter->SetStartAgainIfTimeout(5000);
    taskEnter->onBegin = [ped]() {
        if(ped->isEnteringCar) return;

        TASK_FOLLOW_FOOTSTEPS(ped->ref, GetPlayerActor());
    };
    taskEnter->onExecute = [ped, taskEnter]() {        
        if(!Peds::IsValid(ped)) return SCRIPT_CANCEL;

        if(ped->IsInAnyCar()) return SCRIPT_SUCCESS;

        if(taskEnter->totalTimeElapsed > 30000)
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

    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("escort_to_seat_title"));

    {
        auto button = window->AddButton("Banco de tras");
        button->onClick->Add([window, vehicle, ped]() {
            window->Close();

            ped->flags.showWidget = false;
            
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

            ped->EnterVehicle(vehicle->ref, SeatPosition::PASSENGER, lastFreeSeat);
            
            CleoFunctions::AddWaitForFunction("escort_wait_to_enter", [ped]() {
                if(!Peds::IsValid(ped)) return true;
                if(ped->IsInAnyCar()) return true;

                return false;
            }, [ped]() {
                SET_CHAR_STAY_IN_CAR_WHEN_JACKED(ped->ref, true);

                g_escortingPed = nullptr;
                ped->flags.beeingEscorted = true;

                TopMessage::ClearMessage();
                BottomMessage::SetMessage("~y~Leve o ~r~suspeito ~y~para uma base da policia", 5000);
            });
        });
    }
}

bool Escort::IsEscortingSomeone()
{
    return g_escortingPed != nullptr;
}