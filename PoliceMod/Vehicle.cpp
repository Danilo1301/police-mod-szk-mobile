#include "Vehicle.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Peds.h"
#include "Pullover.h"

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;
}

Vehicle::~Vehicle()
{

}

void Vehicle::Update()
{
    auto position = GetPosition();
    auto playerPosition = GetPlayerPosition();
    float distanceToPlayer = distanceBetweenPoints(playerPosition, position);

    if (!isWidgetVisible && Pullover::IsVehicleBeeingPulledOver(this) && distanceToPlayer < 5.0f && !Pullover::IsPulloverMenuOpen())
    {
        widgetOptions = menuSZK->CreateWidgetButton(
            500, 500,
            getPathFromMenuAssets("widget_background1.png"),
            getPathFromAssets("widget_frisk_car.png")
        );
        isWidgetVisible = true;

        widgetOptions->onClickWidget->Add([this]() {
            Pullover::OpenVehicleMenu(this);
        });
    }
    else if (isWidgetVisible && (!Pullover::IsVehicleBeeingPulledOver(this) || distanceToPlayer > 6.0f || Pullover::IsPulloverMenuOpen()))
    {
        if (widgetOptions)
        {
            widgetOptions->Destroy();
            widgetOptions = nullptr;
        }
        isWidgetVisible = false;
    }

    if (widgetOptions)
    {
        auto container = widgetOptions->GetContainer();
        container->useWorldPosition = true;
        container->worldPosition = position;
        container->fixPositionToCenter = true;
    }
}

int Vehicle::AddBlip()
{
    if(blip != NO_BLIP) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CAR(ref);
    return blip;
}

void Vehicle::RemoveBlip()
{
    if(blip == NO_BLIP) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = NO_BLIP;
}

CVector Vehicle::GetPosition()
{
    return GetCarPosition(ref);
}

void Vehicle::SetOwners()
{
    hDriver = CleoFunctions::GET_DRIVER_OF_CAR(ref);

    if(hDriver > 0)
    {
        auto ped = Peds::GetPed(hDriver);
        ped->previousSeat = PedSeat::DRIVER;
    }

    // passengers

    hPassengers.clear();

    auto currentPassengers = GetCurrentPassengers();
    for(auto passenger : currentPassengers)
    {
        hPassengers.push_back(passenger->ref);

        auto ped = Peds::GetPed(passenger->ref);
        ped->previousSeat = PedSeat::PASSENGER;
    }
}

std::vector<Ped*> Vehicle::GetOwners()
{
    std::vector<Ped*> owners;

    auto driver = Peds::GetPed(hDriver);

    if(driver != NULL)
    {
        owners.push_back(driver);
    }

    for(int refPassenger : hPassengers)
    {
        auto ped = Peds::GetPed(refPassenger);

        if(ped == NULL) continue;

        owners.push_back(ped);
    }

    return owners;
}

Ped* Vehicle::GetCurrentDriver()
{
    auto refDriver = CleoFunctions::GET_DRIVER_OF_CAR(ref);

    if(refDriver <= 0)
    {
        return NULL;
    }

    return Peds::GetPed(refDriver);
}

std::vector<Ped*> Vehicle::GetCurrentPassengers()
{
    std::vector<Ped*> passengers;

    auto maxPassengers = CleoFunctions::CAR_MAX_PASSENGERS(ref);

    for(int seatId = 0; seatId < maxPassengers; seatId++)
    {
        if(!CleoFunctions::CAR_PASSENGER_SEAT_FREE(ref, seatId))
        {
            auto passengerHandle = CleoFunctions::GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(ref, seatId);
            auto ped = Peds::GetPed(passengerHandle);
            passengers.push_back(ped);
        }
    }

    return passengers;
}

std::vector<Ped*> Vehicle::GetCurrentOccupants()
{
    auto occupants = GetCurrentPassengers();

    auto driver = GetCurrentDriver();
    if(driver != NULL)
    {
        occupants.push_back(driver);
    }

    return occupants;
}

void Vehicle::MakeOwnersEnter()
{
    auto owners = GetOwners();

    int passengerSeatId = 0;
    for(auto ped : owners)
    {
        ped->StopHandsup();

        if(ped->previousSeat == PedSeat::DRIVER)
        {
            ped->EnterVehicle(ref, PedSeat::DRIVER, 0);
        } else {
            ped->EnterVehicle(ref, ped->previousSeat, passengerSeatId);
            passengerSeatId++;
        }
    }
}