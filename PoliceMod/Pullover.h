#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Pullover {
public:
    static void Initialize();
    static void Update();
    static int GetCharPlayerIsAiming();

    static void PulloverPed(Ped* ped, bool doAction = true);
    static void FreePed(Ped* ped);
    static void RemovePedFromPullover(Ped* ped);
    static void FreeVehicle(Vehicle* vehicle);
    static void RemoveVehicleFromPullover(Vehicle* vehicle);

    static void TryPulloverFromVehicle();
    static void TryPulloverVehicle();
    static void PulloverVehicle(Vehicle* vehicle);

    static bool IsPedBeeingPulledOver(Ped* ped);
    static bool IsVehicleBeeingPulledOver(Vehicle* vehicle);

    static void OpenVehicleMenu(Vehicle* vehicle);
    static void OpenPedMenu(Ped* ped);

    static void AskSomeoneToGetVehicle(Vehicle* vehicle);
    static void CallTowTruck(Vehicle* vehicle);
};