#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Criminals {
public:
    static void Update();
    static void AddCriminal(Ped* ped);
    static void RemoveCriminal(Ped* ped);
    static bool IsCriminal(Ped* ped);
    static std::vector<Ped*> GetCriminals();
    static std::vector<Vehicle*> GetCriminalsVehicles();
};