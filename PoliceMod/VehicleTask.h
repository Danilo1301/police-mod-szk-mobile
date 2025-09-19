#pragma once

#include "pch.h"
#include "Vehicle.h"

class VehicleTask {
private:
    Vehicle* vehicle;
public:
    VehicleTask(Vehicle* vehicle);

    void DriveTo(CVector position, std::function<void()> onArrive);
};