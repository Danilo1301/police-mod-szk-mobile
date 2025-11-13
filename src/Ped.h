#pragma once

#include "pch.h"

#include "Inventory.h"
#include "Checkpoint.h"

struct ePedFlags
{
    bool hasSurrended = false;
    bool willSurrender = true;
    bool willKillCops = false;

    bool isInconcious = false;
    bool showWidget = false;
    bool beeingEscorted = false;
    int copActingOnPed = 0;
    bool shownRG = false;
    bool wantedByJustice = false;
    bool expiredDriversLicense = false;
    int targetPed = -1;

    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;

    bool showBackCheckpoint = false;

    bool canShowFriskMenu = false;
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

    std::string rg;
    std::string cpf;

    std::string catHab;

    Inventory inventory;

    Checkpoint* backCheckpoint = nullptr;

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

    void DestroyImmediate();
    void QueueDestroy();

    bool IsDeadOrInconcious();

    void TryInitializeInventory();

    void OnEnterBackCheckpoint();
};

inline std::string randomRG() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    ss << dist(gen) << dist(gen) << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-" << dist(gen);

    return ss.str();
}

inline std::string randomCPF() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 2; ++i) ss << dist(gen);

    return ss.str();
}

inline std::string randomCatHab()
{
    std::string cat;

    // 50% chance de ter A
    if (calculateProbability(0.50f))
        cat += "A";

    // 90% chance de ter B
    if (calculateProbability(0.90f))
    {
        cat += "B";

        // 8% chance de ter C
        if (calculateProbability(0.08f))
            cat += "C";

        // 3% chance de ter D
        if (calculateProbability(0.03f))
            cat += "D";

        // 3% chance de ter E
        if (calculateProbability(0.03f))
        {
            cat += "E";
        }
    }
        
    if (cat.empty())
        cat = "B"; // fallback, sempre tem pelo menos B

    return cat;
}