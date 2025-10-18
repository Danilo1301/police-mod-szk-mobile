#include "Vehicle.h"

#include "Pullover.h"
#include "CleoFunctions.h"
#include "WorldWidgets.h"
#include "Peds.h"
#include "Vehicles.h"

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    {
        auto widget = menuSZK->CreateWidget(
            200,
            200,
            100,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );
        //widget->visible = false;

        WorldWidget w;
        w.widget = widget;
        w.attachToVehicle = ref;
        WorldWidgets.push_back(w);

        widget->onClick->Add([this]() {
            Pullover::OpenVehicleMenu(this);
        });

        widgetOptions = widget;
    }

    flags.isStolen = calculateProbability(0.2f);

    if(flags.isStolen)
    {
        ShowBlip(CRGBA(50, 50, 50, 100));
    }
}

Vehicle::~Vehicle()
{
    if(widgetOptions)
    {
        widgetOptions->Close();
        widgetOptions = nullptr;
    }
}

void Vehicle::Update()
{
    timeAlive += menuSZK->deltaTime;

    auto carPosition = GetPosition();
    auto playerPosition = *g_playerPosition;

    if(widgetOptions)
    {
        bool widgetVisible = false;

        if(distanceBetweenPoints(carPosition, playerPosition) < 10.0f)
        {
            widgetVisible = flags.showWidget;
        }

        widgetOptions->visible = widgetVisible;
    }
 
    if(!hasInitializedPeds)
    {
        TryInitializePedsInside();
    }
}

void Vehicle::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Vehicle::HideBlip()
{
    flags.showBlip = false;
}

CVector Vehicle::GetPosition()
{
    return GetCarPosition(ref);
}

bool Vehicle::IsPlayerInside()
{
    return false;
}

int Vehicle::GetCurrentDriver()
{
    auto refDriver = GET_DRIVER_OF_CAR(ref);
    return refDriver;
}

bool Vehicle::HasDriver()
{
    auto driver = GetCurrentDriver();
    return driver > 0;
}

std::vector<int> Vehicle::GetCurrentPassengers()
{
    std::vector<int> passengers;

    auto maxPassengers = CAR_MAX_PASSENGERS(ref);

    for(int seatId = 0; seatId < maxPassengers; seatId++)
    {
        if(!CAR_PASSENGER_SEAT_FREE(ref, seatId))
        {
            auto passengerHandle = GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(ref, seatId);
            passengers.push_back(passengerHandle);
        }
    }

    return passengers;
}

std::vector<int> Vehicle::GetCurrentOccupants()
{
    std::vector<int> occupants;

    auto driverRef = GetCurrentDriver();
    if (driverRef > 0)
    {
        occupants.push_back(driverRef);
    }

    auto passengers = GetCurrentPassengers();
    occupants.insert(occupants.end(), passengers.begin(), passengers.end());

    return occupants;
}

void Vehicle::MakeOccupantsLeave()
{
    SetOwners();

    auto ocuppants = GetCurrentOccupants();

    for(auto pedRef : ocuppants)
    {
        auto ped = Peds::GetPed(pedRef);
        ped->LeaveCar();
    }
}

void Vehicle::MakeOccupantsEnter()
{
    auto owners = GetOwners();

    int passengerSeatId = 0;
    for(auto pedRef : owners)
    {
        auto ped = Peds::GetPed(pedRef);

        if(ped->prevSeatPosition == SeatPosition::DRIVER)
        {
            ped->EnterVehicle(ref, SeatPosition::DRIVER, 0);
        } else {
            ped->EnterVehicle(ref, ped->prevSeatPosition, passengerSeatId);
            passengerSeatId++;
        }
    }
}

void Vehicle::SetOwners()
{
    ownerPassengers.clear();
    ownerDriver = -1;

    auto driverRef = GetCurrentDriver();
    if(driverRef > 0)
    {
        ownerDriver = driverRef;

        auto ped = Peds::GetPed(driverRef);
        ped->vehicleOwned = ref;
    }

    auto passengersRef = GetCurrentPassengers();
    for(auto passengerRef : passengersRef)
    {
        ownerPassengers.push_back(passengerRef);
    }

    TryInitializePedsInside();
}

std::vector<int> Vehicle::GetOwners()
{
    std::vector<int> owners;

    if(ACTOR_DEFINED(ownerDriver))
    {
        owners.push_back(ownerDriver);
    }

    for(auto passengerRef : ownerPassengers)
    {
        if(!ACTOR_DEFINED(passengerRef)) continue;

        owners.push_back(passengerRef);
    }

    return owners;
}

void Vehicle::DestroySelfAndPeds()
{
    auto occupants = GetCurrentOccupants();

    for(auto pedRef : occupants)
    {
        DESTROY_ACTOR(pedRef);
        Peds::RemovePed(pedRef);
    }

    DESTROY_CAR(ref);
    Vehicles::RemoveVehicle(ref);
}

void Vehicle::TryInitializePedsInside()
{
    if(hasInitializedPeds) return;
    hasInitializedPeds = true;

    auto ocuppants = GetCurrentOccupants();

    if(ocuppants.size() > 0)
    {
        for(auto pedRef : ocuppants)
        {
            auto ped = Peds::GetPed(pedRef);
            ped->InitializeOnVehicle(ref);
        }
    }
}