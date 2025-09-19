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

    std::string name = "Maria Sem Nome";
    std::string birthDate = "00/00/0000";
    std::string rg = "12-123-123-5";
    std::string cpf = "123-123-123-52";
    std::string catHab = "";
    std::string cnhExpireDate = "";

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