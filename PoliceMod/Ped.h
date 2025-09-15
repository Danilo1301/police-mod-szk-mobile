#pragma once

#include "pch.h"

#include "menuSZK/IWidgetButton.h"

enum PedSeat
{
    NO_SEAT = 0,
    DRIVER,
    PASSENGER
};

class Ped {
private:
    bool isWidgetVisible = false;
    IWidgetButton* widgetOptions = nullptr;
public:
    int ref;
    void* ptr;
    
    int blip = NO_BLIP;

    int pulledOverFromVehicle = 0;

    bool doHandsup = false;

    PedSeat previousSeat = PedSeat::NO_SEAT;

    Ped(int ref, void* ptr);
    ~Ped();

    void Update();

    void DoHandsup();
    void StopHandsup();
    bool IsDoingHandsupAnim();
    bool IsInAnyCar();

    CVector GetPosition();

    void StartDrivingRandomly();

    void LeaveCar();
    void EnterVehicle(int vehicleRef, PedSeat seat, int seatId);

    int AddBlip();
    void RemoveBlip();
};