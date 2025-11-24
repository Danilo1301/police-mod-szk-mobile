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
    static std::vector<BackupUnit> helicopterBackupUnits;

    static std::vector<Vehicle*> backupVehicles;

    static void Initialize();
    static void LoadBackups();
    static void LoadBackupsFromFolder(const std::string& folder, std::vector<BackupUnit>& outVector);
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
    static void SpawnMedicUnit(CVector position);
};

inline bool IsHelicopterBackupUnit(int modelId)
{
    return std::any_of(
        BackupUnits::helicopterBackupUnits.begin(),
        BackupUnits::helicopterBackupUnits.end(),
        [modelId](const BackupUnit& u) {
            return u.vehicleModelId == modelId;
        }
    );
}

inline bool IsBackupUnit(int modelId)
{
    return std::any_of(
        BackupUnits::defaultBackupUnits.begin(),
        BackupUnits::defaultBackupUnits.end(),
        [modelId](const BackupUnit& u) {
            return u.vehicleModelId == modelId;
        }
    );
}