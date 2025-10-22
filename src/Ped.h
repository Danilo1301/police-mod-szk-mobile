#pragma once

#include "pch.h"

struct ePedFlags
{
    bool hasSurrended = false;
    bool willSurrender = true;
    bool isInconcious = false;
    bool showWidget = false;
    bool beeingEscorted = false;
    int copActingOnPed = 0;

    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
};

enum SeatPosition
{
    NONE,
    DRIVER,
    PASSENGER
};

class Ped {
private:
    bool wasAlive = false;

    std::string currentAnim = "";
    std::string currentAnimGroup = "";
public:
    int ref;
    void* ptr;

    bool isLeavingCar = false;
    bool isEnteringCar = false;
    
    ePedFlags flags;

    IWidget* widgetOptions = nullptr;

    SeatPosition prevSeatPosition = SeatPosition::NONE;
    int previousVehicle = -1;

    int vehicleOwned = -1;

    Ped(int ref, void* ptr);
    ~Ped();

    void Update();

    void SetCanDoHandsup();
    void PerformAnims();
    bool IsPerformingAnimation(const std::string& animName);

    void ShowBlip(CRGBA color);
    void HideBlip();

    CVector GetPosition();

    bool IsInAnyCar();
    int GetCurrentCar();
    bool IsDriver();
    void LeaveCar();

    void UpdateSeatPosition();

    void EnterVehicle(int vehicleRef, SeatPosition seat, int seatId);

    void StartDrivingRandomly();

    void SetAnim(std::string anim, std::string animGroup);
    void ClearAnim();

    void InitializeOnVehicle(int vehicleRef);

    void CopyFrom(const Ped& other);

    void Reanimate();

    void DestroySelf();
};