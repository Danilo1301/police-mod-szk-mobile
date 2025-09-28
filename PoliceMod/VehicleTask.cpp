#include "VehicleTask.h"

#include "CleoFunctions.h"
#include "Vehicles.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

VehicleTask::VehicleTask(Vehicle* vehicle)
{
    this->vehicle = vehicle;
}

void VehicleTask::DriveTo(CVector position, std::function<void()> onArrive)
{
    auto vehicleRef = vehicle->ref;

    SET_CAR_MAX_SPEED(vehicleRef, 20.0f);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicleRef, DrivingMode::AvoidCars);

    CAR_DRIVE_TO(vehicleRef, position.x, position.y, position.z);

    CleoFunctions::AddWaitForFunction([this, position] () {

        if(!Vehicles::IsValid(vehicle))
        {
            return true;
        }

        auto distance = distanceBetweenPoints(vehicle->GetPosition(), position);

        debug->AddLine("distance to target: " + std::to_string(distance));

        if(distance < 10.0f)
        {
            return true;
        }

        return false;
    }, [this, onArrive] () {
        onArrive();
    });
}