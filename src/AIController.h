#pragma once

#include "pch.h"

#include "Vehicle.h"
#include "AIVehicle.h"

class AIController {
public:
    static std::map<Vehicle*, AIVehicle*> vehicleAIs;

    static void AddAIToVehicle(Vehicle* vehicle, AIVehicle* ai);
    static void RemoveAIsFromVehicle(Vehicle* vehicle);

    static void Update();
};