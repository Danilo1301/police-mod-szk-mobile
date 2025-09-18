#pragma once

#include "pch.h"

#include "menuSZK/IWidgetButton.h"
#include "Ped.h"

class Vehicle {
private:
    bool isWidgetVisible = false;
    IWidgetButton* widgetOptions = nullptr;
    bool plateHidden = false;
public:
    int ref;
    void* ptr;
    
    int blip = NO_BLIP;

    int hDriver;
    std::vector<int> hPassengers;

    std::string plate;

    Vehicle(int ref, void* ptr);
    ~Vehicle();

    void Update();

    void OnRenderBefore();
    void OnRenderAfter();

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