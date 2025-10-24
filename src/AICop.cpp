#include "AICop.h"

#include "CleoFunctions.h"
#include "Criminals.h"
#include "Peds.h"
#include "Vehicles.h"

AICop::~AICop()
{

}

void AICop::Start()
{
    fileLog->Log("AICop: Start");
    menuDebug->AddLine("AICop started");

    g_onPedLeaveVehicle->Add([this](int pedRef) {

        fileLog->Log("Ped left vehicle");

        if(pedRef == this->pedRef)
        {
            DoAction();
        }
    });
} 

void AICop::Update()
{
    if(!ACTOR_DEFINED(pedRef)) return;

    auto cop = Peds::GetPed(pedRef);
    auto copVehicle = Vehicles::GetVehicle(cop->previousVehicle);

    int dt = menuSZK->deltaTime;

    findTargetTimer += dt;

    if (findTargetTimer >= 3000)
    {
        findTargetTimer = 0;
        FindTarget();
    }

    if(!ACTOR_DEFINED(targetPed))
    {
        targetPed = -1;
    }

    if(targetPed != prevTargetPed)
    {
        DoAction();
        prevTargetPed = targetPed;
    }

    if(copVehicle)
    {
        if(copVehicle->flags.drivingToPed != -1)
        {
            driveToTimer += menuSZK->deltaTime;
            
            if(driveToTimer > 2000)
            {
                driveToTimer = 0;
                copVehicle->flags.drivingToPed = -1;
            }
        }
    }

    if(targetPed != -1)
    {
        auto criminal = Peds::GetPed(targetPed);
        auto criminalPosition = criminal->GetPosition();

        auto distanceToCriminal = distanceBetweenPoints(cop->GetPosition(), criminalPosition);

        if(distanceToCriminal > 30.0f)
        {
            if(copVehicle)
            {
                copVehicle->ValidateOwners();

                if(!copVehicle->IsAllOwnersInside())
                {
                    copVehicle->flags.drivingToPed = -1;

                    if(!copVehicle->IsAnyOwnersLeavingOrEnteringCar())
                    {
                        menuDebug->AddLine("~b~entrando no carro");

                        copVehicle->MakeOwnersEnter();
                    }
                }

                if(copVehicle->IsAllOwnersInside())
                {
                    if(copVehicle->flags.drivingToPed != targetPed && cop->IsDriver())
                    {
                        copVehicle->flags.drivingToPed = targetPed;

                        fileLog->Log("Cop drive to target");

                        menuDebug->AddLine("~b~cop driving to target");

                        ENABLE_CAR_SIREN(copVehicle->ref, true);
                        SET_CAR_TRAFFIC_BEHAVIOUR(copVehicle->ref, DrivingMode::AvoidCars);
                        SET_CAR_MAX_SPEED(copVehicle->ref, CHASE_MAX_POLICE_SPEED);

                        if(criminal->IsInAnyCar())
                        {
                            menuDebug->AddLine("~r~suspect is on foot");
                        } else {                            
                            CAR_DRIVE_TO(copVehicle->ref, criminalPosition.x, criminalPosition.y, criminalPosition.z);
                        }
                    }
                }
            }
        }

        if(distanceToCriminal < 30.0f - 2)
        {
            copVehicle->ValidateOwners();
            if(copVehicle && copVehicle->GetCurrentOccupants().size() > 0 && !copVehicle->IsAnyOwnersLeavingOrEnteringCar())
            {
                if(cop->IsDriver())
                {
                    fileLog->Log("cops leaving car");

                    menuDebug->AddLine("~b~saindo do carro");

                    copVehicle->MakeOccupantsLeave();
                }
            }
        }
    }
}

void AICop::FindTarget()
{
    targetPed = -1;
    findTargetTimer = 0;

    // garante que o próprio cop exista
    auto selfPed = Peds::GetPed(pedRef);
    if (!selfPed) return;

    auto criminals = *Criminals::GetCriminals();
    if (criminals.empty()) return;

    Ped* nearestFree = nullptr;   // não surrender / não inconcious
    float nearestFreeDist = std::numeric_limits<float>::max();

    Ped* nearestAny = nullptr;    // qualquer um (fallback)
    float nearestAnyDist = std::numeric_limits<float>::max();

    auto selfPos = selfPed->GetPosition();

    for (auto crim : criminals)
    {
        if (!crim) continue;

        // tenta obter o Ped do criminoso
        auto crimPed = Peds::GetPed(crim->ref);
        if (!crimPed) continue;

        float dist = distanceBetweenPoints(selfPos, crimPed->GetPosition());

        // mantem o mais próximo de todos (fallback)
        if (dist < nearestAnyDist)
        {
            nearestAnyDist = dist;
            nearestAny = crim;
        }
    
        // considera apenas os que NÃO estão surrender ou inconcious
        if (!crim->flags.hasSurrended && !crim->flags.isInconcious)
        {
            if (dist < nearestFreeDist)
            {
                nearestFreeDist = dist;
                nearestFree = crim;
            }
        }
    }

    // prefere o nearestFree; caso não exista, usa nearestAny
    if (nearestFree)
        targetPed = nearestFree->ref;
    else if (nearestAny)
        targetPed = nearestAny->ref;
    // caso não tenha nenhum válido, targetPed já é -1
}

void AICop::DoAction()
{
    auto cop = Peds::GetPed(pedRef);

    if(cop->isEnteringCar || cop->isLeavingCar) return;
    
    FindTarget();

    menuDebug->AddLine("~b~cop do action");

    fileLog->Log("Cop do action");

    

    if(targetPed != -1)
    {
        auto criminal = Peds::GetPed(targetPed);

        if(!cop->IsInAnyCar())
        {
            if(criminal->flags.willSurrender)
            {
                TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);
                AIM_AT_ACTOR(pedRef, targetPed, 8000);
            } else {

                if(criminal->flags.willKillCops)
                {
                    KILL_ACTOR(pedRef, targetPed);
                } else {
                    TASK_FOLLOW_FOOTSTEPS(pedRef, targetPed);

                    menuDebug->AddLine("~r~criminal is running likea coward");
                    FLEE_FROM_ACTOR(targetPed, pedRef, 200.0f, -1);
                }
            }
        }
    }
}