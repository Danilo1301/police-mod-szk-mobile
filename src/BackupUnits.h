#pragma once

#include "pch.h"

#include "Vehicle.h"
#include "AudioCollection.h"

struct BackupUnit
{
    std::string name = "";
    int vehicleModelId;
    int skinModelId;
    int occupants = 2;
    float chance = 1.0f;
};

struct RoadName
{
    AudioVariationGroup* audioGroup;
    CVector position;
};

class BackupUnits {
public:
    static std::vector<BackupUnit> defaultBackupUnits;

    static std::vector<Vehicle*> backupVehicles;

    static void Initialize();
    static void LoadBackups();
    static void InitializeRoads();
    static void Update();
    static void OnPostDrawRadar();
    static void CheckIfVehiclesAreValid();
    static void SendQTH();
    static void PlayRoadName();
    static void SpawnBackupUnit(BackupUnit* unit);
    static void SpawnRandomBackupUnit();
    static void AddVehicleAsBackup(Vehicle* vehicle, bool recreatePeds);
    static BackupUnit* GetRandomUnitByChance(std::vector<BackupUnit>& units);
    static void OpenSpawnBackupMenu();
    static void SpawnMedicUnit();
};