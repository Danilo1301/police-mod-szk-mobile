#pragma once

#include "pch.h"

#include "Ped.h"
#include "Vehicle.h"

class DocsWindow {
public:
    static void ShowRG(Ped* ped);
    static void ShowRGResults(Ped* ped);

    static void ShowVehicleDocs(Vehicle* vehicle);
    static void ShowVehicleResults(Vehicle* vehicle);

    static void ShowCNH(Ped* ped);

};