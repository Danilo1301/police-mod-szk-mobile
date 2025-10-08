#include "BackupAI.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Criminals.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "ScriptTask.h"
#include "BackupUnits.h"
#include "Pullover.h"

void BackupAI::Update()
{
    //logDebug("BackupAI::Update");

    if(!vehicle)
    {
        logInternal("ERROR: vehicle is null");
    }

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

    if (followingPed == nullptr)
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
    carIsInRange = false;
    if(criminalVehicle)
    {
        carIsInRange = distanceBetweenPoints(vehicle->GetPosition(), targetPosition) < 10.0f;
    } else {
        carIsInRange = distanceBetweenPoints(vehicle->GetPosition(), targetPosition) < 20.0f;
    }

    // checa velocidade do alvo
    const bool vehicleIsTooSlow = criminalVehicle ? CAR_SPEED(criminalVehicle->ref) < 5.0f : false;

    // lógica de desembarque
    if (carIsInRange && (vehicleIsTooSlow || criminalVehicle == nullptr) && everyoneIsOnVehicle && !leavingVehicle)
    {
        leavingVehicle = true;
        debug->AddLine("peds desembarcando...");
        logInternal("backup peds leaving...");

        vehicle->SetOwners();

        for (auto ped : vehicle->GetCurrentOccupants())
        {
            ped->LeaveCar();
        }

        if(!followingPed->hasSurrended)
        {
            if(calculateProbability(0.30))
            {
                debug->AddLine("~y~driver has surrended");

                if(criminalVehicle)
                {
                    auto driver = criminalVehicle->GetCurrentDriver();
                    driver->LeaveCar();
                    Pullover::PulloverPed(driver, false);

                    auto passengers = criminalVehicle->GetCurrentPassengers();
                    for(auto ped : passengers)
                    {
                        ped->LeaveCar();
                        
                        if(calculateProbability(0.70))
                        {
                            Pullover::PulloverPed(ped, false);
                        } else {
                            FLEE_FROM_ACTOR(ped->ref, driver->ref, 40.0f, -1);

                            debug->AddLine("~y~but the supid ahh passenger did not surrend");
                        }
                    }
                } else {
                    Pullover::PulloverPed(followingPed, false);
                }
            }
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

        Follow();
    }
}

void BackupAI::ProcessFollow()
{
    if (!followingPed) return;

    if (everyoneIsOnVehicle && !carIsInRange)
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
    if(!vehicle) return;

    for (auto police : vehicle->GetOwners())
    {
        if (police->justLeftVehicle && followingPed)
        {
            debug->AddLine("~y~AIM at the mf");

            AIM_AT_ACTOR(police->ref, followingPed->ref, 10000);
        }
    }
}

void BackupAI::Follow()
{
    debug->AddLine("backupAi: try follow criminal");
    
    logInternal("following criminal");

    if (!followingPed) return;

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
    logInternal("BackupAI::LeaveAndDestroy");

    if(!vehicle) return;

    auto vehicle = this->vehicle;

    isLeavingScene = true;

    debug->AddLine("backup leaving scene");

    auto ownersCount = vehicle->GetOwners().size();

    vehicle->MakeOwnersEnter();

    CleoFunctions::AddWaitForFunction("bu_wait_pass_toenter", [vehicle, ownersCount] () {
        
        if(!Vehicles::IsValid(vehicle))
        {
            debug->AddLine("vehicle is not valid");
            return true;
        }

        auto ocuppantsCount = vehicle->GetCurrentOccupants().size();

        debug->AddLine("esperando passageiros " + std::to_string(ocuppantsCount) + "/" + std::to_string(ownersCount));

        if(ocuppantsCount >= ownersCount)
        {
            return true;
        }

        return false;
    }, [vehicle] () {

        if(!Vehicles::IsValid(vehicle)) return;

        logInternal("passengers entered");
        
        // lembrar q essa porra deleta o BackupAI*
        logInternal("remove from backup");
        BackupUnits::RemoveVehicleFromBackup(vehicle);

        //vehicle->RemoveBlip();

        ENABLE_CAR_SIREN(vehicle->ref, false);

        ScriptTask* taskLeave = new ScriptTask("bu_taskleave");
        taskLeave->onBegin = [vehicle]() {
            SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
            SET_CAR_TO_PSYCHO_DRIVER(vehicle->ref);
            SET_CAR_MAX_SPEED(vehicle->ref, 20.0f);
        };
        taskLeave->onExecute = [vehicle]() {

            if(!Vehicles::IsValid(vehicle)) return true;

            auto distance = DistanceFromPed(GetPlayerActor(), vehicle->GetPosition());

            if(distance < 120.0f) return false;

            return true;
        };
        taskLeave->onComplete = [vehicle]() {
            if(Vehicles::IsValid(vehicle))
            {
                vehicle->DestroyCarAndPeds();
            }
        };
        taskLeave->Start();
    });
}