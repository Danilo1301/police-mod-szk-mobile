#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"
#include "Checkpoint.h"

class Scorch {
public:
    static void Update();

    static void StartCarry(Ped* ped);
    static void StopCarry();
    static void TestCheckpoint(Checkpoint* checkpoint, Vehicle* vehicle);

    static bool IsCarrying();
    static Ped* GetCarryingPed();
};