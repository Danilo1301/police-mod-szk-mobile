#include "AICriminal.h"

#include "CleoFunctions.h"
#include "BackupUnits.h"
#include "Peds.h"
#include "Vehicles.h"

const float MAX_FIND_TARGET_DISTANCE = 30.0f; // limite em metros

AICriminal::~AICriminal()
{

}

void AICriminal::Start()
{
    fileLog->Log("AICriminal: Start");

    g_onPedLeaveVehicle->Add([this](int pedRef) {

        fileLog->Log("Ped left vehicle");

        if(pedRef == this->pedRef)
        {
            DoAction();
        }
    });
} 

void AICriminal::Update()
{
    if (!ACTOR_DEFINED(pedRef))
        return;
    
    auto criminal = Peds::GetPed(pedRef);

    // --------- target -------------
    
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

    if(targetPed != -1)
    {
        auto distanceToTarget = DistanceFromPed(targetPed, criminal->GetPosition());

        if(distanceToTarget > MAX_FIND_TARGET_DISTANCE)
        {
            targetPed = -1;
            menuDebug->AddLine("~r~Target is too far away");
        }
    }

    //

    bool targetChanged = prevTargetPed != targetPed;
    prevTargetPed = targetPed;

    if(targetChanged)
        DoAction();

    if(criminal->IsInAnyCar() && criminal->IsDriver())
    {
        auto car = Vehicles::GetVehicle(criminal->GetCurrentCar());

        bool isSlow = CAR_SPEED(car->ref) < 2.0f;

        bool isPlayerClose = DistanceFromPed(GetPlayerActor(), criminal->GetPosition()) < 50.0f;

        if(carWasSlow != isSlow)
        {
            carWasSlow = isSlow;

            if(isSlow && isPlayerClose)
            {
                if(calculateProbability(0.50))
                {
                    car->MakeOccupantsLeave();
                }
            }
        }
    }

    if(!criminal->IsInAnyCar() && !criminal->flags.hasSurrended)
    {
        if(criminal->flags.willSurrender == false && criminal->flags.willKillCops == false)
        {
            int copClose = GetClosestCop(15.0f);

            if(copClose > 0)
            {
                surrenderTimer += menuSZK->deltaTime;

                if(surrenderTimer >= timeToSurrender)
                {
                    menuDebug->AddLine("~r~criminal surrended");

                    criminal->flags.hasSurrended = true;
                    criminal->flags.showWidget = true;
                    criminal->SetCanDoHandsup();
                }
            }
        }
    }
}

void AICriminal::FindTarget()
{
    targetPed = -1;
    findTargetTimer = 0;

    auto criminal = Peds::GetPed(pedRef);
    if (!criminal) return;

    auto vehicles = BackupUnits::backupVehicles;
    std::vector<Ped*> cops;

    // adiciona o player também
    cops.push_back(Peds::GetPed(GetPlayerActor()));

    // recolhe todos os policiais dos veículos de backup
    for (auto vehicle : vehicles)
    {
        auto owners = vehicle->GetOwners();
        for (auto ownerRef : owners)
        {
            auto cop = Peds::GetPed(ownerRef);
            if (cop)
                cops.push_back(cop);
        }
    }

    if (cops.empty())
        return;

    auto position = criminal->GetPosition();

    Ped* nearestCop = nullptr;
    float nearestDist = std::numeric_limits<float>::max();

    // acha o policial mais próximo dentro do limite
    for (auto cop : cops)
    {
        if (!cop) continue;

        if(ACTOR_DEAD(cop->ref)) continue;
        if(cop->flags.isInconcious) continue;

        float dist = distanceBetweenPoints(position, cop->GetPosition());
        if (dist < nearestDist && dist <= MAX_FIND_TARGET_DISTANCE)
        {
            nearestDist = dist;
            nearestCop = cop;
        }
    }

    // define o alvo apenas se estiver dentro do limite
    if (nearestCop)
        targetPed = nearestCop->ref;
}


void AICriminal::DoAction()
{
    auto criminal = Peds::GetPed(pedRef);
    
    if(criminal->IsDeadOrInconcious()) return;
    if(criminal->flags.hasSurrended) return;

    if(targetPed != -1)
    {
        menuDebug->AddLine("~r~criminal do action");

        if(!criminal->IsInAnyCar())
        {
            if(!criminal->flags.willSurrender)
            {
                if (criminal->flags.willKillCops)
                {
                    KILL_ACTOR(pedRef, targetPed);

                    menuDebug->AddLine("~r~Criminal is now killing " + std::to_string(targetPed));
                } else {
                    menuDebug->AddLine("~r~Criminal is now running away from " + std::to_string(targetPed));

                    FLEE_FROM_ACTOR(pedRef, targetPed, 200.0f, -1);
                }
            }
        }
    } else {
        FLEE_FROM_ACTOR(pedRef, GetPlayerActor(), 30.0f, -1);
    }
}

int AICriminal::GetClosestCop(float range)
{
    Ped* nearestCop = nullptr;
    float nearestDist = FLT_MAX;

    auto criminal = Peds::GetPed(pedRef);
    if (!criminal) return -1;

    auto position = criminal->GetPosition();

    auto vehicles = BackupUnits::backupVehicles;
    std::vector<Ped*> cops;

    // adiciona o player também
    cops.push_back(Peds::GetPed(GetPlayerActor()));

    // recolhe todos os policiais dos veículos de backup
    for (auto vehicle : vehicles)
    {
        if (!vehicle) continue;

        auto owners = vehicle->GetOwners();
        for (auto ownerRef : owners)
        {
            auto cop = Peds::GetPed(ownerRef);
            if (cop)
                cops.push_back(cop);
        }
    }

    // encontra o mais próximo
    for (auto cop : cops)
    {
        if (!cop) continue;
        if (ACTOR_DEAD(cop->ref)) continue;
        if (cop->flags.isInconcious) continue;

        float dist = distanceBetweenPoints(position, cop->GetPosition());
        if (dist < nearestDist && dist <= range)
        {
            nearestDist = dist;
            nearestCop = cop;
        }
    }

    return nearestCop ? nearestCop->ref : -1;
}