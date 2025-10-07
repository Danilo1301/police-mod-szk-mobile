#pragma once

#include "pch.h"

#include "Ped.h"

class Trunk {
private:
    int vehicleRef;
    std::vector<int> pedsInside;
public:
    Trunk(int vehicleRef);

    void AddPedToTrunk(int pedRef);
    void RemoveAllPeds();

    bool HasPedsInside() { return pedsInside.size() > 0; }
};