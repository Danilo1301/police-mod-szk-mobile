#pragma once

#include "pch.h"

#include "Vehicle.h"
#include "AIVehicle.h"
#include "Ped.h"
#include "AIPed.h"

class AIController {
public:
    static std::map<Vehicle*, AIVehicle*> vehicleAIs;
    static std::map<Ped*, AIPed*> pedsAI;

    static void AddAIToVehicle(Vehicle* vehicle, AIVehicle* ai);
    static void RemoveAIsFromVehicle(Vehicle* vehicle);

    static void AddAIToPed(Ped* ped, AIPed* ai);
    static void RemoveAIsFromPed(Ped* ped);

    static void Update();
};