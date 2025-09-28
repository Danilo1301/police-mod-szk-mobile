#pragma once

#include "pch.h"
#include "Vehicle.h"

class BackupUnits {
public:
    static void Update();
    static bool IsBackupVehicle(Vehicle* vehicle);

    static void AddVehicleAsBackup(Vehicle* vehicle);
    static void RemoveVehicleFromBackup(Vehicle* vehicle);

    static void SpawnRandomBackupUnit();
};