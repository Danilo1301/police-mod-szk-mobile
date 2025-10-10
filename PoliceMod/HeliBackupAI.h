#pragma once

#include "pch.h"

#include "AIController.h"
#include "Vehicle.h"
#include "Ped.h"

class HeliBackupAI : public VehicleAI {
private:
    bool isLeavingScene = false;
    Ped* followingCriminal = nullptr;
public:
    void Update() override;

    void FindNewCriminal();
    void FollowCriminal();

    void Leave();
};