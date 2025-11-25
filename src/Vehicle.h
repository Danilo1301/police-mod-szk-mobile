#pragma once

#include "pch.h"

#include "Checkpoint.h"
#include "Trunk.h"
#include "PoliceVehicleData.h"

inline bool g_canShowCarWidgetAnyTime = false;

struct VehicleDocInfo
{
    std::string plate;
    std::string chassis;
    std::string renavam;
    
    bool isStolen;
    bool isDocumentExpired;

    VehicleDocInfo()
    {
        plate = randomPlate();
        chassis = randomVIN();
        renavam = randomRENAVAM();

        isStolen = false;
        isDocumentExpired = false;
    }
};

struct eVehicleFlags
{
    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
    int drivingToPed = -1;

    bool showWidget = false;
    bool isDrivingAway = false;

    bool chassisErased = false;
    bool swappedPlate = false;
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

    VehicleDocInfo originalDoc;
    VehicleDocInfo currentDoc;

    int ownerDriver = -1;
    std::vector<int> ownerPassengers;

    int timeAlive = 0;
    int timeSinceLastRepair = 0;

    Checkpoint* trunkCheckpoint = nullptr;
    CVector trunkOffset = CVector(0, -4.0f, 0);

    Trunk* trunk;

    PoliceVehicleData* policeVehicleData = nullptr;

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
    bool GetSeatThatPedBelongs(int pedRef, int& seatId);
    static int GetCurrentSeatOfPed(int carRef, int pedRef);

    void MakeOccupantsLeave();
    void MakeOwnersEnter();

    void SetOwners();
    std::vector<int> GetOwners();
    void ValidateOwners();

    void DestroyImmediate();
    void QueueDestroy(bool destroyOccupants = false);

    void TryInitializePedsInside();

    int GetModelId();

    bool IsAllOwnersInside();
    bool IsAnyOwnersLeavingOrEnteringCar();

    float GetSpeed();

    CRGBA GetBlipColor();

    void RemoveReferences();
};