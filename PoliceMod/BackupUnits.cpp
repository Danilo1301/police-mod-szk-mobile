#include "BackupUnits.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "Vehicles.h"
#include "CleoFunctions.h"
#include "Chase.h"
#include "AIController.h"
#include "BackupAI.h"
#include "Peds.h"
#include "Globals.h"
#include "ModelLoader.h"
#include "Criminals.h"
#include "BottomMessage.h"
#include "Globals.h"
#include "SpriteUtils.h"

std::vector<Vehicle*> backupVehicles;

int timeToSpawnBackup = 10000;
CVector lastQTHPosition = CVector(0, 0, 0);
float maxDistanceToQTH = 300.0f;

void BackupUnits::Update()
{
    bool isInChase = Chase::InChase;
    bool isCloseToQTH = distanceBetweenPoints(lastQTHPosition, GetPlayerPosition()) < maxDistanceToQTH;

    for (auto it = backupVehicles.begin(); it != backupVehicles.end(); )
    {
        Vehicle* vehicle = *it;
        if (!Vehicles::IsValid(vehicle))
        {
            backupVehicles.erase(it); // retorna o próximo válido

            it = backupVehicles.begin();

            AIController::RemoveAIsFromVehicle(vehicle);
            break;
        }
        else
        {
            ++it; // só avança se não apagou
        }
    }

    if(isInChase)
    {
        auto vehicles = Vehicles::GetVehiclesMap();
        for (auto pair : vehicles)
        {
            auto vehicle = pair.second;

            if(!IsVehicleModelAPoliceCar(vehicle->GetModelId())) continue;
            if(!vehicle->spawnedWithDriver) continue;
            if(vehicle->IsPlayerInside()) continue;

            if(vehicle->lifeTime < 1000)
            {
                vehicle->DestroyCarAndPeds();
                debug->AddLine("~y~npc police vehicle spawned and destroyed");
                continue;
            }
        }
    }

    if(isInChase && isCloseToQTH && backupVehicles.size() < 8)
    {
        timeToSpawnBackup -= menuSZK->deltaTime;

        if(timeToSpawnBackup <= 0)
        {
            timeToSpawnBackup = getRandomNumber(5000, 10000);

            debug->AddLine("spawning random backup unit");

            SpawnRandomBackupUnit();
        }
    }

    for(auto chaseVehicle : Criminals::GetCriminalsVehicles())
    {
        auto vehiclesInSphere = Vehicles::GetAllCarsInSphere(chaseVehicle->GetPosition(), 100.0f);

        for (auto* vehicle : vehiclesInSphere)
        {
            if (!IsVehicleModelAPoliceCar(vehicle->GetModelId())) continue;
            if (vehicle->GetCurrentDriver() == nullptr) continue;
            if (vehicle->IsInChase()) continue;
            if (IsBackupVehicle(vehicle)) continue;
            if (vehicle->IsPlayerInside()) continue;

            AddVehicleAsBackup(vehicle);
        }
    }
}

void BackupUnits::OnDrawRadar()
{
    auto sprite = SpriteUtils::spriteBigCircle;

    if (sprite == nullptr)
    {
        debug->AddLine("~r~its fucking null");
        return;
    }

    if (!sprite->loaded)
    {
        debug->AddLine("~r~its not fucking loaded");
        return;
    }

    auto playerPosition = GetPlayerPosition();
    auto distance = distanceBetweenPoints(lastQTHPosition, playerPosition);

    const float fadeStart = maxDistanceToQTH - 80.0f;
    const float fadeEnd = maxDistanceToQTH;

    unsigned char alpha = 0;

    if (distance <= fadeStart)
    {
        alpha = 100; // totalmente visível até 200m
    }
    else if (distance >= fadeEnd)
    {
        alpha = 0; // invisível a partir de 300m
    }
    else
    {
        // interpolar suavemente entre 200 e 300
        float t = (distance - fadeStart) / (fadeEnd - fadeStart);
        alpha = static_cast<unsigned char>(100 * (1.0f - t));
    }

    auto color = COLOR_POLICE;
    color.a = alpha;

    SpriteUtils::DrawSpriteInRadarWorld(sprite, playerPosition, color, 150.0f);
}

bool BackupUnits::IsBackupVehicle(Vehicle* vehicle)
{
    return std::any_of(backupVehicles.begin(), backupVehicles.end(),
                       [vehicle](Vehicle* v){ return v == vehicle; });
}

void BackupUnits::AddVehicleAsBackup(Vehicle* vehicle)
{
    if(IsBackupVehicle(vehicle)) return;

    REMOVE_REFERENCES_TO_CAR(vehicle->ref);
    // for(auto ped : vehicle->GetCurrentOccupants())
    // {
    //     REMOVE_REFERENCES_TO_ACTOR(ped->ref);
    // }

    auto oldDriver = vehicle->GetCurrentDriver();
    auto driverModelId = GET_ACTOR_MODEL(oldDriver->ref);
    
    DESTROY_ACTOR(oldDriver->ref);
    Peds::RemovePed(oldDriver->ref);

    auto newDriverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicle->ref, PedType::Special, driverModelId);
    auto driver = Peds::RegisterPed(newDriverRef);

    int pistolId = 22;

    GIVE_ACTOR_WEAPON(driver->ref, pistolId, 5000);

    backupVehicles.push_back(vehicle);

    vehicle->SetOwners();

    AIController::AddAIToVehicle(vehicle, new BackupAI());
}

void BackupUnits::RemoveVehicleFromBackup(Vehicle* vehicle)
{
    if (!IsBackupVehicle(vehicle)) return;

    backupVehicles.erase(
        std::remove(backupVehicles.begin(), backupVehicles.end(), vehicle),
        backupVehicles.end()
    );

    AIController::RemoveAIsFromVehicle(vehicle);
}

void BackupUnits::SpawnRandomBackupUnit()
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
        auto driver = Peds::RegisterPed(driverRef);

        AddVehicleAsBackup(car);
    });

}

void BackupUnits::SendQTH()
{
    lastQTHPosition = GetPlayerPosition();

    BottomMessage::SetMessage("QTH atualizado", 3000);
}

void BackupUnits::ResetQTH()
{
    lastQTHPosition = CVector(0, 0, 0);
}