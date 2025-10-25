#include "BackupUnits.h"

#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "ModelLoader.h"
#include "Vehicles.h"
#include "Peds.h"
#include "AIController.h"
#include "AIBackupVehicle.h"
#include "Criminals.h"
#include "AICop.h"

std::vector<Vehicle*> BackupUnits::backupVehicles;

void BackupUnits::Initialize()
{
    g_onVehicleDestroy->Add([](int vehicleRef) {
        auto vehicle = Vehicles::GetVehicle(vehicleRef);
        if (!vehicle) return;

        auto it = std::find(backupVehicles.begin(), backupVehicles.end(), vehicle);
        if (it != backupVehicles.end())
            backupVehicles.erase(it);
    });
}

void BackupUnits::SendQTH()
{
    fileLog->Log("BackupUnits: SendQTH");
    
    BottomMessage::SetMessage("Apoio solicitado", 3000);

    if(Criminals::GetCriminals()->size() == 0)
    {
        BottomMessage::SetMessage("~r~Nao foi possivel encontrar nenhum suspeito", 3000);
        return;
    }
    
    SpawnBackupUnit();
}

void BackupUnits::SpawnBackupUnit()
{
    fileLog->Log("BackupUnits: SpawnBackupUnit");

    auto closePosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 80, 0));

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

        auto passengerRef = CREATE_ACTOR_PEDTYPE_IN_CAR_PASSENGER_SEAT(carRef, PedType::Special, pedModel, 0);
        Peds::RegisterPed(passengerRef);

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

    fileLog->Log("Setup occupants");

    auto occupants = vehicle->GetCurrentOccupants();
    for(auto pedRef : occupants)
    {
        auto cop = Peds::GetPed(pedRef);

        int pistolId = 22;

        GIVE_ACTOR_WEAPON(pedRef, pistolId, 5000);

        cop->ShowBlip(COLOR_POLICE);

        auto ai = new AICop();
        AIController::AddAIToPed(cop, ai);
        ai->Start();
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
        //auto ai = new AIBackupVehicle();
        
        //AIController::AddAIToVehicle(vehicle, ai);

        //ai->Start();
    }
}