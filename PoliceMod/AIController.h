#pragma once

#include "pch.h"

#include "Vehicle.h"

struct VehicleAI {
public:
    Vehicle* vehicle = nullptr;

    virtual void Update();
};

class AIController {
public:
    static std::map<Vehicle*, VehicleAI*> vehicleAIs;

    static void AddAIToVehicle(Vehicle* vehicle, VehicleAI* ai);
    static void RemoveAIsFromVehicle(Vehicle* vehicle);

    static void Update();
};