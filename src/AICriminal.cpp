#include "AICriminal.h"

#include "CleoFunctions.h"
#include "BackupUnits.h"
#include "Peds.h"

const float MAX_FIND_TARGET_DISTANCE = 30.0f; // limite em metros

AICriminal::~AICriminal()
{

}

void AICriminal::Start()
{
    fileLog->Log("AICriminal: Start");
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
        OnTargetChange();

    
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


void AICriminal::OnTargetChange()
{
    menuDebug->AddLine("Criminal OnTargetChange");

    auto criminal = Peds::GetPed(pedRef);

    if(criminal->IsDeadOrInconcious()) return;

    if (criminal->flags.willKillCops)
    {
        if(targetPed != -1)
        {
            KILL_ACTOR(pedRef, targetPed);

            menuDebug->AddLine("~r~Criminal is now killing " + std::to_string(targetPed));
        }
    }
}