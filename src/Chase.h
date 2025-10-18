#pragma once

#include "pch.h"

#include "Vehicle.h"

#define CHASE_MAX_VEHICLE_SPEED 40.0f

class Chase {
public:
    static void StartChaseWithVehicle(Vehicle* vehicle);

    static void Update();
};