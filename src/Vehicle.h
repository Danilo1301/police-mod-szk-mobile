#pragma once

#include "pch.h"

struct eVehicleFlags
{
    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
    bool isStolen = false;

    bool showWidget = false;
};

class Vehicle {
private:
    bool hasInitializedPeds = false;
    int modelId = 0;
public:
    int ref;
    void* ptr;

    eVehicleFlags flags;

    IWidget* widgetOptions = nullptr;

    int ownerDriver = -1;
    std::vector<int> ownerPassengers;

    int timeAlive = 0;
    int timeSinceLastRepair = 0;

    Vehicle(int ref, void* ptr);
    ~Vehicle();

    void Update();

    void ShowBlip(CRGBA color);
    void HideBlip();

    void RemoveBlips();

    CVector GetPosition();

    bool IsPlayerInside();

    int GetCurrentDriver();
    bool HasDriver();
    std::vector<int> GetCurrentPassengers();
    std::vector<int> GetCurrentOccupants();

    void MakeOccupantsLeave();
    void MakeOwnersEnter();

    void SetOwners();
    std::vector<int> GetOwners();

    void DestroySelfAndPeds();

    void TryInitializePedsInside();

    int GetModelId();

    bool IsAllOwnersInside();

    float GetSpeed();
};