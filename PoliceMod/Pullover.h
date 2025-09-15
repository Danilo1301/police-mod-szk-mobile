#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Pullover {
public:
    static void Initialize();
    static void Update();
    static int GetCharPlayerIsAiming();

    static void PulloverPed(Ped* ped);
    static void FreePed(Ped* ped);
    static void FreeVehicle(Vehicle* vehicle);

    static void TryPulloverFromVehicle();
    static void TryPulloverVehicle();
    static void PulloverVehicle(Vehicle* vehicle);

    static bool IsPedBeeingPulledOver(Ped* ped);
    static bool IsVehicleBeeingPulledOver(Vehicle* vehicle);

    static void OpenPedMenu(Ped* ped);
    static void OpenVehicleMenu(Vehicle* vehicle);

    static bool IsPulloverMenuOpen();
};