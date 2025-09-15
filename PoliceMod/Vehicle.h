#pragma once

#include "pch.h"

#include "menuSZK/IWidgetButton.h"
#include "Ped.h"

class Vehicle {
private:
    bool isWidgetVisible = false;
    IWidgetButton* widgetOptions = nullptr;
public:
    int ref;
    void* ptr;
    
    int blip = NO_BLIP;

    int hDriver;
    std::vector<int> hPassengers;

    Vehicle(int ref, void* ptr);
    ~Vehicle();

    void Update();

    int AddBlip();
    void RemoveBlip();

    CVector GetPosition();

    void SetOwners();
    std::vector<Ped*> GetOwners();

    Ped* GetCurrentDriver();
    std::vector<Ped*> GetCurrentPassengers();
    std::vector<Ped*> GetCurrentOccupants();

    void MakeOwnersEnter();
};