#include "HeliBackupAI.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Criminals.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "ScriptTask.h"
#include "BackupUnits.h"
#include "Pullover.h"

void HeliBackupAI::Update()
{
    if(!vehicle)
    {
        logInternal("ERROR: vehicle is null");
    }

    auto criminals = Criminals::GetCriminals();

    if(criminals.size() == 0 && !isLeavingScene)
    {
        Leave();
        return;
    }

    if(isLeavingScene) return;

    auto closestCriminal = Criminals::GetClosestCriminal(GetPlayerPosition());
    if (followingCriminal != closestCriminal)
    {
        followingCriminal = closestCriminal;
        FollowCriminal();
    }
}

void HeliBackupAI::FindNewCriminal()
{
    auto closestCriminal = Criminals::GetClosestCriminal(GetPlayerPosition());

    if (closestCriminal)
    {
        followingCriminal = closestCriminal;
    }
    else
    {
        followingCriminal = nullptr;
    }
}

void HeliBackupAI::FollowCriminal()
{
    if(!vehicle) return;
    if(!followingCriminal) return;

    logInternal("heli follow criminal");
    debug->AddLine("heli follow ~r~criminal");

    HELI_FOLLOW(vehicle->ref, followingCriminal->ref, -1, 20.0f);
}

void HeliBackupAI::Leave()
{
    logInternal("HeliBackupAI::Leave");

    isLeavingScene = true;

    if(!vehicle) return;

    auto vehicle = this->vehicle;

    ScriptTask* taskLeave = new ScriptTask("heli_leave");
    taskLeave->onBegin = [vehicle]() {

        auto position = vehicle->GetPosition();
        auto flyTo = position + CVector(1000.0f, 0, 0);

        HELI_FLY_TO(vehicle->ref, flyTo.x, flyTo.y, 200.0f, 200.0f, 200.0f);
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
}