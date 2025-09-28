#pragma once

#include "pch.h"

#include "AIController.h"
#include "Vehicle.h"
#include "Ped.h"

class BackupAI : public VehicleAI {
private:
    bool everyoneIsOnVehicle = false;
    bool leavingVehicle = false;
    bool enteringVehicle = false;
    int timeToFollow = 0;
    bool carIsInRange = false;
    bool isLeavingScene = false;
public:
    Ped* followingPed = nullptr;

    void Update() override;

    void ProcessFollow();
    void ProcessPeds();
    void Follow();
    void LeaveAndDestroy();
};