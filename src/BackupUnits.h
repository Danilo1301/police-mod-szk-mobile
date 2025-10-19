#pragma once

#include "pch.h"

#include "Vehicle.h"

class BackupUnits {
public:
    static std::vector<Vehicle*> backupVehicles;

    static void SendQTH();
    static void SpawnBackupUnit();
    static void AddVehicleAsBackup(Vehicle* vehicle, bool recreatePeds);
};