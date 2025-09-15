#pragma once

#include "pch.h"

#include "Vehicle.h"

class Vehicles {
private:
    static std::map<int, Vehicle*> vehicles;
public:
    static void AddVehicle(int ref, void* ptr);
    static void RemoveVehicle(int ref);
    static Vehicle* GetVehicle(int ref);
    static void Update();
    static std::vector<Vehicle*> GetAllCarsInSphere(CVector position, float radius);
    static Vehicle* GetClosestVehicle(CVector position, float radius);
};