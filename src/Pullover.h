#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class Pullover {
public:
    static void OnClickWidget();

    static void TryPulloverFromVehicle();
    static void TryPulloverOnFoot();

    static void PulloverPed(Ped* ped);
    static void FreePed(Ped* ped);

    static void PulloverVehicle(Vehicle* vehicle);

    static int FindAimingPed();

    static void OpenPedMenu(Ped* ped);
};