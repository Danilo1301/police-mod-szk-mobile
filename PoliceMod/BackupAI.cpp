#include "BackupAI.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Criminals.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "ScriptTask.h"
#include "BackupUnits.h"

void BackupAI::Update()
{
    logInternal("BackupAI::Update");

    // contagem de donos e ocupantes
    const auto ownersCount    = vehicle->GetOwners().size();
    const auto occupantsCount = vehicle->GetCurrentOccupants().size();

    // pega criminosos
    auto criminals = Criminals::GetCriminals();
    if (criminals.empty())
    {
        followingPed = nullptr;
        debug->AddLine("no criminals");
    }
    else
    {
        if (!followingPed) followingPed = criminals[0];
    }

    everyoneIsOnVehicle = (occupantsCount == ownersCount) && ownersCount > 0;
    carIsInRange        = false;

    ProcessPeds();
    ProcessFollow();

    if (!followingPed)
    {
        if(!isLeavingScene)
        {
            LeaveAndDestroy();
            return;
        }
        return;
    }

    // calcula posição do alvo (veículo ou pedestre)
    Vehicle* criminalVehicle = Vehicles::GetVehicle(GetVehiclePedIsUsing(followingPed->ref));
    CVector targetPosition   = criminalVehicle ? criminalVehicle->GetPosition() 
                                               : followingPed->GetPosition();

    // checa proximidade
    carIsInRange = (distanceBetweenPoints(vehicle->GetPosition(), targetPosition) < 10.0f);

    // checa velocidade do alvo
    const bool vehicleIsTooSlow = (criminalVehicle && CAR_SPEED(criminalVehicle->ref) < 5.0f);

    // lógica de desembarque
    if (carIsInRange && vehicleIsTooSlow && everyoneIsOnVehicle && !leavingVehicle)
    {
        leavingVehicle = true;
        debug->AddLine("peds desembarcando...");
        logInternal("backup peds leaving...");

        for (auto ped : vehicle->GetCurrentOccupants())
        {
            ped->LeaveCar();
        }
        return;
    }

    // lógica de embarque
    if (!carIsInRange && occupantsCount == 0 && !enteringVehicle)
    {
        enteringVehicle = true;
        debug->AddLine("peds embarcando...");

        logInternal("backup peds entering");

        vehicle->MakeOwnersEnter();
    }

    // resets de estado
    if (leavingVehicle && occupantsCount == 0)
    {
        leavingVehicle = false;
        debug->AddLine("looks like everyone got out");
    }

    if (enteringVehicle && everyoneIsOnVehicle)
    {
        enteringVehicle = false;
        debug->AddLine("looks like everyone got in");
    }
}

void BackupAI::ProcessFollow()
{
    if (!followingPed) return;

    if (everyoneIsOnVehicle)
    {
        timeToFollow -= menuSZK->deltaTime;
        if (timeToFollow <= 0)
        {
            timeToFollow = 5000; // próximo follow em 5s
            Follow();
        }
    }
    else
    {
        timeToFollow = 500; // aguarda menos tempo quando não estão no veículo
    }
}

void BackupAI::ProcessPeds()
{
    for (auto police : vehicle->GetOwners())
    {
        if (police->justLeftVehicle && followingPed)
        {
            debug->AddLine("~y~AIM at the mf");
        }
    }
}

void BackupAI::Follow()
{
    logInternal("following criminal");

    if (!followingPed) return;

    debug->AddLine("backupAi: follow criminal");

    if (IS_CHAR_IN_ANY_CAR(followingPed->ref))
    {
        auto chaseVehicle = Vehicles::GetVehicle(GetVehiclePedIsUsing(followingPed->ref));
        if (!chaseVehicle || !chaseVehicle->ref) return;

        CAR_FOLLOW_CAR(vehicle->ref, chaseVehicle->ref, 8.0f);
    }
    else
    {
        auto pedPosition = followingPed->GetPosition();
        CAR_DRIVE_TO(vehicle->ref, pedPosition.x, pedPosition.y, pedPosition.z);
    }
    
    ENABLE_CAR_SIREN(vehicle->ref, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
    SET_CAR_MAX_SPEED(vehicle->ref, CHASE_MAX_VEHICLE_SPEED);
}

void BackupAI::LeaveAndDestroy()
{
    isLeavingScene = true;

    BackupUnits::RemoveVehicleFromBackup(vehicle);

    debug->AddLine("backup leaving scene");

    vehicle->RemoveBlip();

    ENABLE_CAR_SIREN(vehicle->ref, false);

    ScriptTask* taskLeave = new ScriptTask();
    taskLeave->onBegin = [this]() {
        SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
        SET_CAR_TO_PSYCHO_DRIVER(vehicle->ref);
        SET_CAR_MAX_SPEED(vehicle->ref, 20.0f);
    };
    taskLeave->onExecute = [this]() {

        if(!Vehicles::IsValid(vehicle)) return true;

        auto distance = DistanceFromPed(GetPlayerActor(), vehicle->GetPosition());

        if(distance < 120.0f) return false;

        return true;
    };
    taskLeave->onComplete = [this]() {
        if(Vehicles::IsValid(vehicle))
        {
            vehicle->DestroyCarAndPeds();
        }
    };
    taskLeave->Start();
}