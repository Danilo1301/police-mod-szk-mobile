#pragma once

#include "pch.h"

#include "Ped.h"

class Escort {
public:
    static void Update();
    static void EscortPed(Ped* ped);
    static void OnPlayerEnterVehicle();
    static bool IsEscortingSomeone();
};