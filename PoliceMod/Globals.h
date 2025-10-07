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

static CRGBA COLOR_YELLOW = CRGBA(255, 255, 0);
static CRGBA COLOR_CRIMINAL = CRGBA(255, 60, 60);
static CRGBA COLOR_POLICE = CRGBA(0, 150, 255);