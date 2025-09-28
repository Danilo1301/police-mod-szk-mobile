#pragma once

#include "pch.h"

#include "Vehicle.h"

class Chase {
public:
    static bool InChase;

    static void StartChaseWithVehicle(Vehicle* vehicle);

    static void Update();
};