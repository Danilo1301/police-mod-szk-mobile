#include "BackupUnits.h"

#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "ModelLoader.h"
#include "Vehicles.h"
#include "Peds.h"
#include "AIController.h"
#include "AIBackupVehicle.h"

std::vector<Vehicle*> BackupUnits::backupVehicles;

void BackupUnits::SendQTH()
{
    BottomMessage::SetMessage("Apoio solicitado", 3000);

    SpawnBackupUnit();
}

void BackupUnits::SpawnBackupUnit()
{
    auto closePosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(80, 50, 0));

    auto spawnPosition = GET_CLOSEST_CAR_NODE(closePosition.x, closePosition.y, closePosition.z);

    int vehicleModel = 596;
    int pedModel = 280;

    ModelLoader::AddModelToLoad(vehicleModel);
    ModelLoader::AddModelToLoad(pedModel);
    ModelLoader::LoadAll([vehicleModel, pedModel, spawnPosition]() {
        auto carRef = CREATE_CAR_AT(vehicleModel, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto car = Vehicles::RegisterVehicle(carRef);

        auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carRef, PedType::Special, pedModel);
        Peds::RegisterPed(driverRef);

        AddVehicleAsBackup(car, false);
    });
}

void BackupUnits::AddVehicleAsBackup(Vehicle* vehicle, bool recreatePeds)
{
    fileLog->Log("BackupUnits: AddVehicleAsBackup");

    //REMOVE_REFERENCES_TO_CAR(vehicle->ref);

    if(recreatePeds)
    {
        auto oldDriverRef = vehicle->GetCurrentDriver();

        if(oldDriverRef > 0)
        {
            auto oldDriver = Peds::GetPed(oldDriverRef);

            auto driverModelId = GET_ACTOR_MODEL(oldDriver->ref);
            
            DESTROY_ACTOR(oldDriver->ref);
            Peds::RemovePed(oldDriver->ref);

            auto newDriverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicle->ref, PedType::Special, driverModelId);
            Peds::RegisterPed(newDriverRef);
        }
    }

    auto occupants = vehicle->GetCurrentOccupants();
    for(auto pedRef : occupants)
    {
        auto cop = Peds::GetPed(pedRef);

        int pistolId = 22;

        GIVE_ACTOR_WEAPON(pedRef, pistolId, 5000);
        REMOVE_REFERENCES_TO_ACTOR(pedRef);

        cop->ShowBlip(COLOR_POLICE);
    }

    backupVehicles.push_back(vehicle);

    vehicle->SetOwners();
    vehicle->ShowBlip(COLOR_POLICE);

    //auto modelId = vehicle->GetModelId();

    bool isPoliceHelicopter = false;

    if(isPoliceHelicopter)
    {
        // auto ai = new HeliBackupAI();

        // AIController::AddAIToVehicle(vehicle, ai);

        // ai->FindNewCriminal();
        // ai->FollowCriminal();
    } else {
        auto ai = new AIBackupVehicle();
        
        AIController::AddAIToVehicle(vehicle, ai);

        ai->Start();
    }
}