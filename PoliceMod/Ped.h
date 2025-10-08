#pragma once

#include "pch.h"

#include "menuSZK/IWidgetButton.h"

#include "dialog/DialogManager.h"
#include "inventory/Inventory.h"

enum PedInVehicleState
{
    NONE,
    SITTING,
    LEAVING,
    ENTERING,
    NOT_ON_VEHICLE
};

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
    bool doHandsup = false;
    bool doCover = false;
public:
    int ref;
    void* ptr;

    CRGBA mapIconColor = CRGBA(255, 255, 255, 0);

    int pulledOverFromVehicle = 0;

    std::string name = "Maria Sem Nome";
    std::string birthDate = "00/00/0000";
    std::string rg = "12-123-123-5";
    std::string cpf = "123-123-123-52";
    std::string catHab = "";
    std::string cnhExpireDate = "";

    PedSeat previousSeat = PedSeat::NO_SEAT;

    PedInVehicleState inVehicleState = PedInVehicleState::NONE;
    bool justLeftVehicle = false;
    bool justEnteredVehicle = false;

    bool hasSurrended = false;

    Dialogue dialogue;

    Inventory inventory;

    Ped(int ref, void* ptr);
    ~Ped();

    void CopyFrom(const Ped& other) {
        int oldRef = ref;
        void* oldPtr = ptr;

        *this = other; // reutiliza o operador=

        this->ref = oldRef;
        this->ptr = oldPtr;
        this->widgetOptions = nullptr;
    }

    void Update();

    void DoHandsup();
    void StopHandsup();
    bool IsDoingHandsupAnim();

    void DoCover();
    void StopCover();
    bool IsDoingCoverAnim();
    
    bool IsInAnyCar();

    

    CVector GetPosition();
    void SetPosition(CVector position);

    void StartDrivingRandomly();

    void LeaveCar();
    void EnterVehicle(int vehicleRef, PedSeat seat, int seatId);

    void SetMapIconColor(CRGBA color);
    void HideMapIcon();

    bool HasCNH() { return catHab != ""; }

    bool IsCriminal();
};