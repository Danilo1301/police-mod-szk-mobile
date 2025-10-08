#include "ScriptTask.h"

#include "ModelLoader.h"
#include "CleoFunctions.h"
#include "Peds.h"
#include "BottomMessage.h"
#include "Objectives.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

ScriptTask::ScriptTask(std::string name)
{
    this->name = name;
}

void ScriptTask::Start()
{
    debug->AddLine("task : " + name + " : start");

    if(onBegin) onBegin();

    CleoFunctions::AddWaitForFunction(name, [this]() {
        timePassedToTimeout += menuSZK->deltaTime;
        if(timeout > 0)
        {
            if(timePassedToTimeout >= timeout)
            {
                timePassedToTimeout = 0;
                if(onBegin) onBegin();
            }
        }

        if(onExecute)
        {
            completed = onExecute();
        } else {
            completed = true;
        }

        return this->completed;
    }, [this]() {
        auto fn = onComplete;

        debug->AddLine("task : " + name + " : completed");

        if(deleteOnComplete)
            delete this;

        if(fn) fn();
    });
}

void ScriptTask::SetStartAgainIfTimeout(int timeout)
{
    this->timeout = timeout;
}

void ScriptTask::MakeVehicleLeave(int vehicleRef)
{
    auto startPosition = GetCarPosition(vehicleRef);

    auto targetPosition = GET_CLOSEST_CAR_NODE(startPosition.x, startPosition.y + 1000, startPosition.z);

    SET_CAR_MAX_SPEED(vehicleRef, 20.0f);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicleRef, DrivingMode::StopForCarsIgnoreLights);
    CAR_DRIVE_TO(vehicleRef, targetPosition.x, targetPosition.y, targetPosition.z);
}

void ScriptTask::CreateTest()
{
    int pedModelId = 40;

    ModelLoader::AddModelToLoad(pedModelId);

    ModelLoader::LoadAll([pedModelId]() {
        auto playerActor = GetPlayerActor();
        auto playerPosition = GetPedPosition(playerActor);

        auto spawnPosition = STORE_PED_PATH_COORDS_CLOSEST_TO(playerPosition.x, playerPosition.y, playerPosition.z);
        auto targetPosition = spawnPosition;
        targetPosition.x += 10;

        auto pedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, pedModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto ped = Peds::RegisterPed(pedRef);

        //

        ScriptTask* taskWalkToAnotherPoint = new ScriptTask("test_walk");
        taskWalkToAnotherPoint->SetStartAgainIfTimeout(8000);

        taskWalkToAnotherPoint->onBegin = [pedRef, targetPosition]() {
            BottomMessage::SetMessage("Rodando task", 1000);
            TASK_GO_TO_COORD_ANY_MEANS(pedRef, targetPosition.x, targetPosition.y, targetPosition.z, 6, false);
        };
        taskWalkToAnotherPoint->onExecute = [taskWalkToAnotherPoint, pedRef, targetPosition]() {
            
            auto distance = DistanceFromPed(pedRef, targetPosition);

            debug->AddLine("distance to " + CVectorToString(targetPosition) + " is " + std::to_string(distance));

            if(distance > 3.0f)
            {
                return false;
            }

            return true;
        };
        taskWalkToAnotherPoint->onComplete = [ped]() {
            
            BottomMessage::SetMessage("Tarefa completada", 3000);
            Objectives::ClearObjective();
            
        };
        taskWalkToAnotherPoint->Start();

        Objectives::SetObjective("Aguarde o ped chegar ao destino");
    });
}