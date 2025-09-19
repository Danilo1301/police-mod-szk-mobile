#pragma once

#include "pch.h"

#include "Ped.h"

class Peds {
private:
    static std::map<int, Ped*> peds;
public:
    static void AddPed(int ref, void* ptr);
    static Ped* RegisterPed(int ref);
    static void RemovePed(int ref);
    static Ped* GetPed(int ref);
    static void Update();
};