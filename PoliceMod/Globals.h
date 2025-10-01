#pragma once

#include "pch.h"

#include "menuSZK/IMenuSZK.h"

class Globals {
public:
    static std::vector<int> policeVehicleIds;
};

static bool IsVehicleModelAPoliceCar(int modelId)
{
    return std::find(
        Globals::policeVehicleIds.begin(),
        Globals::policeVehicleIds.end(),
        modelId
    ) != Globals::policeVehicleIds.end();
}
