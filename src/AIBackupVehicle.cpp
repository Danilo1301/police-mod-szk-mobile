#include "AIBackupVehicle.h"

#include "Criminals.h"
#include "CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"

AIBackupVehicle::~AIBackupVehicle()
{

}

void AIBackupVehicle::Start()
{
    menuDebug->AddLine("so, AI started");
   
    FindCriminal();
    GotoCriminal();
} 

void AIBackupVehicle::Update()
{
    if(!CAR_DEFINED(vehicleRef)) return;

    //criminal

    if(criminalRef > 0 && !ACTOR_DEFINED(criminalRef))
    {
        criminalRef = 0;
        return;
    }
    
    auto criminal = criminalRef > 0 ? Peds::GetPed(criminalRef) : nullptr;

    // destroy

    bool canDestroy = false;
    
    auto distance = DistanceFromVehicle(vehicleRef, GetPlayerPosition());
    if(distance > 500.0f) canDestroy = true;

    if(canDestroy)
    {
        auto vehicle = Vehicles::GetVehicle(vehicleRef);
        vehicle->DestroySelfAndPeds();
        return;
    }

    // find

    ProcessFindCriminalTimer();

    //

    auto vehiclePosition = GetCarPosition(vehicleRef);
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    if(criminal)
    {
        auto distance = distanceBetweenPoints(criminal->GetPosition(), vehiclePosition);

        bool isSlowOrStopped = vehicle->GetSpeed() < 2.0f;

        if(distance < 10 && vehicle->GetCurrentOccupants().size() > 0 && isSlowOrStopped)
        {
            if(!copsAreLeaving)
            {
                copsAreLeaving = true;
                vehicle->MakeOccupantsLeave();
            }
        }

        if(distance > 15 && vehicle->GetCurrentOccupants().size() == 0)
        {
            if(!copsAreEntering)
            {
                copsAreEntering = true;
                vehicle->MakeOwnersEnter();
            }
        }
    }

    if(copsAreLeaving)
    {
        if(vehicle->GetCurrentOccupants().size() == 0)
        {
            copsAreLeaving = false;
        }
    }

    if(copsAreEntering)
    {
        if(vehicle->IsAllOwnersInside())
        {
            copsAreEntering = false;
        }
    }

    // cops

    auto owners = vehicle->GetOwners();
    for(auto copRef : owners)
    {
        auto cop = Peds::GetPed(copRef);

        if(!criminal) cop->flags.copActingOnPed = false;
        if(cop->IsInAnyCar()) cop->flags.copActingOnPed = false;

        if(criminal)
        {
            if(!cop->IsInAnyCar())
            {
                //auto bestCriminalRef = FindBestCriminal(cop->GetPosition());

                if(cop->flags.copActingOnPed != criminal->ref)
                {
                    cop->flags.copActingOnPed = criminal->ref;

                    menuDebug->AddLine("~b~cop taking actions");

                    if(criminal->flags.willSurrender)
                    {
                        TASK_FOLLOW_FOOTSTEPS(copRef, criminal->ref);
                    } else {
                        KILL_ACTOR(copRef, criminal->ref);
                    }
                }
            }
        }
    }
}

void AIBackupVehicle::FindCriminal()
{
    auto criminals = Criminals::GetCriminals();

    menuDebug->AddLine("found ~r~" + std::to_string(criminals->size()) + " criminals");

    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    criminalRef = FindBestCriminal(vehicle->GetPosition());

    // 3️⃣ Se achou, definir como alvo
    if (criminalRef)
    {
        menuDebug->AddLine("selected criminal ~g~" + std::to_string(criminalRef));
    }
    else
    {
        menuDebug->AddLine("~r~no valid criminals found");
    }
}

void AIBackupVehicle::GotoCriminal()
{
    if(copsAreLeaving) return;
    if(criminalRef <= 0) return;
    if(!ACTOR_DEFINED(criminalRef)) return;
    if(!CAR_DEFINED(vehicleRef)) return;

    auto vehicle = Vehicles::GetVehicle(vehicleRef);
    auto criminal = Peds::GetPed(criminalRef);

    menuDebug->AddLine("ai: goto criminal");

    //auto distance = distanceBetweenPoints(criminal->GetPosition(), vehicle->GetPosition());

    if(!vehicle->IsAllOwnersInside())
    {
        menuDebug->AddLine("~r~but they are not inside");

        // if(distance > 10)
        // {
        //     vehicle->MakeOwnersEnter();
        // }

        return;
    }
    
    if (IS_CHAR_IN_ANY_CAR(criminal->ref))
    {
        auto chaseVehicle = Vehicles::GetVehicle(GetVehiclePedIsUsing(criminal->ref));
        if (!chaseVehicle || !chaseVehicle->ref) return;

        CAR_FOLLOW_CAR(vehicle->ref, chaseVehicle->ref, 8.0f);
    }
    else
    {
        auto pedPosition = criminal->GetPosition();
        CAR_DRIVE_TO(vehicle->ref, pedPosition.x, pedPosition.y, pedPosition.z);
    }
    
    ENABLE_CAR_SIREN(vehicle->ref, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
    SET_CAR_MAX_SPEED(vehicle->ref, CHASE_MAX_POLICE_SPEED);
}

bool AIBackupVehicle::CriminalExists()
{
    if(criminalRef <= 0) return false;
    return ACTOR_DEFINED(criminalRef);
}

int AIBackupVehicle::FindBestCriminal(CVector position)
{
    auto criminals = Criminals::GetCriminals();

    if (criminals->empty()) return 0;

    Ped* bestCriminal = nullptr;
    float bestDist = FLT_MAX;

    // 1️⃣ Procurar criminoso mais perto entre os que não vão se render
    for (auto criminal : *criminals)
    {
        if (!criminal) continue;
        //if (criminal->flags.willSurrender) continue;
        if (criminal->flags.hasSurrended) continue;

        float dist = distanceBetweenPoints(position, criminal->GetPosition());
        if (dist < bestDist)
        {
            bestDist = dist;
            bestCriminal = criminal;
        }
    }

    // 2️⃣ Se não achou nenhum “não rendido”, pegar o mais próximo de todos
    if (!bestCriminal)
    {
        for (auto criminal : *criminals)
        {
            if (!criminal) continue;

            float dist = distanceBetweenPoints(position, criminal->GetPosition());
            if (dist < bestDist)
            {
                bestDist = dist;
                bestCriminal = criminal;
            }
        }
    }

    return bestCriminal->ref;
}

void AIBackupVehicle::ProcessFindCriminalTimer()
{
    if(Criminals::GetCriminals()->size() == 0) return;

    int dt = menuSZK->deltaTime; // tempo entre frames

    findCriminalTimer += dt;

    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    auto isOwnersInside = vehicle->IsAllOwnersInside();
    auto time = isOwnersInside ? 8000 : 3000;

    if (findCriminalTimer >= time)
    {
        findCriminalTimer = 0;
        FindCriminal();
        GotoCriminal();
    }
}