#pragma once

#include "pch.h"

struct eVehicleFlags
{
    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
};

class Vehicle {
private:

public:
    int ref;
    void* ptr;

    eVehicleFlags flags;

    IWidget* widgetOptions = nullptr;

    Vehicle(int ref, void* ptr);
    ~Vehicle();

    void Update();

    void ShowBlip(CRGBA color);
    void HideBlip();

    CVector GetPosition();

    bool IsPlayerInside();
};