#pragma once

#include "pch.h"

struct eVehicleFlags
{
    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
    bool isStolen = false;
    bool hasExpiredDocument = false;

    bool showWidget = false;
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

    int ownerDriver = -1;
    std::vector<int> ownerPassengers;

    int timeAlive = 0;
    int timeSinceLastRepair = 0;

    std::string plate;

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

    void MakeOccupantsLeave();
    void MakeOwnersEnter();

    void SetOwners();
    std::vector<int> GetOwners();

    void DestroySelfAndPeds();

    void TryInitializePedsInside();

    int GetModelId();

    bool IsAllOwnersInside();

    float GetSpeed();

    CRGBA GetBlipColor();
};

inline std::string randomPlate()
{
    // Letras permitidas (pode adicionar/remover)
    static const std::vector<std::string> prefixes = {
        "EGC", "BRA", "MNT", "CPU", "LVR", "SJP", "FGT", "QXZ", "HJK", "VPE", 
        "NDO", "RSC", "YAB", "ZGM", "PQL", "TJS", "WFM", "LXN", "CBF", "GVN"
    };

    // RNG (random device + Mersenne Twister)
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Sorteia prefixo
    std::uniform_int_distribution<> prefixDist(0, prefixes.size() - 1);
    std::string plate = prefixes[prefixDist(gen)];

    // Sorteia números (0000–9999)
    std::uniform_int_distribution<> numDist(0, 9999);
    int number = numDist(gen);

    // Formata para sempre ter 4 dígitos
    char buf[10];
    snprintf(buf, sizeof(buf), " %04d", number);

    plate += buf;
    return plate;
}

inline std::string randomPlateLimited()
{
    // Letras permitidas para o prefixo
    static const std::vector<char> letters = {'A', 'C', 'E', 'P'};
    // Números permitidos para os dígitos
    static const std::vector<char> numbers = {'3', '4', '6', '7', '8'};

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distLetters(0, letters.size() - 1);
    std::uniform_int_distribution<> distNumbers(0, numbers.size() - 1);

    std::string plate;

    // 3 letras
    for (int i = 0; i < 3; ++i)
        plate += letters[distLetters(gen)];

    plate += " "; // espaço antes dos números

    // 4 números
    for (int i = 0; i < 4; ++i)
        plate += numbers[distNumbers(gen)];

    return plate;
}