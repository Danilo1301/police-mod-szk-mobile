#include "Chase.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Vehicles.h"
#include "Peds.h"
#include "Criminals.h"
#include "BackupUnits.h"

bool Chase::InChase = false;

void Chase::StartChaseWithVehicle(Vehicle* vehicle)
{
    logToFile("Chase: StartChaseWithVehicle");

    if(vehicle->IsInChase()) return;

    auto oldDriver = vehicle->GetCurrentDriver();

    if(oldDriver == nullptr) return;

    InChase = true;

    auto driverModelId = GET_ACTOR_MODEL(oldDriver->ref);

    logToFile("remove char from car");

    REMOVE_CHAR_FROM_CAR_MAINTAIN_POSITION(oldDriver->ref, vehicle->ref);
    
    logToFile("create new driver");

    auto newDriverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(vehicle->ref, PedType::CivMale, driverModelId);
    auto driver = Peds::RegisterPed(newDriverRef);

    logToFile("copy");

    driver->CopyFrom(*oldDriver);
    
    logToFile("set owners");

    logToFile("destroy");

    DESTROY_ACTOR(oldDriver->ref);
    Peds::RemovePed(oldDriver->ref);

    logToFile("make it run");

    SET_CAR_TRAFFIC_BEHAVIOUR(vehicle->ref, DrivingMode::AvoidCars);
    SET_CAR_TO_PSYCHO_DRIVER(vehicle->ref);
    SET_CAR_MAX_SPEED(vehicle->ref, CHASE_MAX_VEHICLE_SPEED);

    logToFile("finished");

    vehicle->SetOwners();

    for(auto ped : vehicle->GetCurrentOccupants())
    {
        Criminals::AddCriminal(ped);
    }
}

void Chase::Update()
{
    if(InChase)
    {
        auto criminals = Criminals::GetCriminals();
        if(criminals.size() == 0)
        {
            InChase = false;

            debug->AddLine("~y~not in chase anymore");
        }
    }
}