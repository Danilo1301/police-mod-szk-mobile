#pragma once

#include "pch.h"

#include "menuSZK/IWidgetButton.h"
#include "Ped.h"
#include "Checkpoint.h"
#include "Trunk.h"

class Vehicle {
public:
    static bool CanShowWidgetAnyTime;
private:
    bool isWidgetVisible = false;
    IWidgetButton* widgetOptions = nullptr;
    bool plateHidden = false;
    bool isPoliceCar = false;
public:
    int ref;
    void* ptr;
    
    CRGBA mapIconColor = CRGBA(255, 255, 255, 0);

    int hDriver;
    std::vector<int> hPassengers;

    std::string plate;
    std::string chassis;
    std::string renavamCode;
    int exerciseYear;
    int manufactureYear;
    int modelYear;

    bool outdatedDoc = false;

    int lifeTime = 0;

    bool spawnedWithDriver = false;

    int timeSinceLastRepair = 0;

    CVector trunkOffset = CVector(0, -4.0f, 0);
    Checkpoint* trunkCheckpoint = nullptr;

    Trunk* trunk;

    bool isStolen = false;
    bool hasDifferentPlate = false;

    bool showChassis = false;

    Vehicle(int ref, void* ptr);
    ~Vehicle();

    void Update();
    void OnDraw();

    void OnRenderBefore();
    void OnRenderAfter();

    void SetMapIconColor(CRGBA color);
    void HideMapIcon();

    CVector GetPosition();

    void SetOwners();
    std::vector<Ped*> GetOwners();
    void RemoveOwners();

    Ped* GetCurrentDriver();
    std::vector<Ped*> GetCurrentPassengers();
    std::vector<Ped*> GetCurrentOccupants();
    bool IsPlayerInside();
    bool IsPedInside(int pedRef);

    void MakeOwnersEnter();

    int GetModelId();

    void DestroyCarAndPeds();

    bool IsInChase();

    bool IsPoliceCar() { return isPoliceCar; };
};