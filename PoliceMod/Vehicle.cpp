#include "Vehicle.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Peds.h"
#include "Vehicles.h"
#include "Pullover.h"
#include "PoliceMod.h"
#include "VehicleDummy.h"
#include "Chase.h"
#include "Criminals.h"

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    plate = randomPlateLimited();
    chassis = gerarChassi();
    renavamCode = gerarRenavam();

    int anoAtual = getCurrentYear();
    
    manufactureYear = getRandomNumber(anoAtual - 20, anoAtual);
    modelYear = getRandomNumber(manufactureYear, manufactureYear + 1);
    
    outdatedDoc = calculateProbability(0.50f);

    // decide se está atrasado
    if (outdatedDoc)
    {
        // queremos um exerciseYear entre manufactureYear e anoAtual - 1
        int latest = std::max(manufactureYear, anoAtual - 1);

        // se manufactureYear == anoAtual (veículo do ano), forçamos pequeno atraso
        if (latest < manufactureYear) latest = manufactureYear;

        exerciseYear = getRandomNumber(manufactureYear, latest);
    }
    else
    {
        // em dia
        exerciseYear = anoAtual;
    }
}

Vehicle::~Vehicle()
{

}

void Vehicle::Update()
{
    lifeTime = std::min(lifeTime + (int)menuSZK->deltaTime, 100000);

    auto position = GetPosition();
    auto playerPosition = GetPlayerPosition();
    float distanceToPlayer = distanceBetweenPoints(playerPosition, position);

    auto isMenuOpen = PoliceMod::m_IsUsingMenu;

    if (!isWidgetVisible && Pullover::IsVehicleBeeingPulledOver(this) && distanceToPlayer < 5.0f && !isMenuOpen)
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
    else if (isWidgetVisible && (!Pullover::IsVehicleBeeingPulledOver(this) || distanceToPlayer > 6.0f || isMenuOpen))
    {
        if (widgetOptions)
        {
            widgetOptions->Close();
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

    auto carHealth = GET_CAR_HEALTH(ref);
    if(carHealth < 300)
    {
        SET_CAR_HEALTH(ref, 300);
        debug->AddLine("~y~car health restored");
    }
}

void Vehicle::OnRenderBefore()
{
    auto vehicle = (CVehicle*)ptr;

    auto playerCar = ACTOR_USED_CAR(GetPlayerActor());

    if(playerCar != ref) return;

    //debug->AddLine("~b~atomics");

    auto atomics = VehicleDummy::RpClumpGetAllAtomics(vehicle->m_pRwClump);
    for(auto atomic : atomics)
    {
        auto frameAtomic = GetObjectParent((RwObject*)atomic);
		auto name = to_lower(GetFrameName(frameAtomic));

        //debug->AddLine(name);

        if(!atomic->geometry) continue;
        
        if (name.find("plate") != std::string::npos)
        {
            //if(plateHidden) continue;
            // plateHidden = true;

            // debug->AddLine("~y~plate hidden");
            
            // HideAllAtomics(frameAtomic);
        }
    }
}

void Vehicle::OnRenderAfter()
{

}

int Vehicle::AddBlip()
{
    if(blip != NO_BLIP) RemoveBlip();
    blip = ADD_BLIP_FOR_CAR(ref);
    return blip;
}

int Vehicle::AddBlip(int color)
{
    int blip = AddBlip();
    SET_MARKER_COLOR_TO(blip, color);
    return blip;
}

void Vehicle::RemoveBlip()
{
    if(blip == NO_BLIP) return;

    DISABLE_MARKER(blip);
    blip = NO_BLIP;
}

CVector Vehicle::GetPosition()
{
    return GetCarPosition(ref);
}

void Vehicle::SetOwners()
{
    hDriver = GET_DRIVER_OF_CAR(ref);

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

    if(driver != nullptr)
    {
        owners.push_back(driver);
    }

    for(int refPassenger : hPassengers)
    {
        auto ped = Peds::GetPed(refPassenger);

        if(ped == nullptr) continue;

        owners.push_back(ped);
    }

    return owners;
}

void Vehicle::RemoveOwners()
{
    if(hDriver >= 0)
    {
        auto ped = Peds::GetPed(hDriver);
        if(ped)
        {
            ped->pulledOverFromVehicle = 0;
        }
    }
    hDriver = 0;

    // passengers

    for(auto hPassenger : hPassengers)
    {
        auto ped = Peds::GetPed(hPassenger);

        if(ped)
        {
            ped->pulledOverFromVehicle = 0;
        }
    }
    hPassengers.clear();
}

Ped* Vehicle::GetCurrentDriver()
{
    auto refDriver = GET_DRIVER_OF_CAR(ref);

    if(refDriver < 0)
    {
        return NULL;
    }

    return Peds::GetPed(refDriver);
}

std::vector<Ped*> Vehicle::GetCurrentPassengers()
{
    std::vector<Ped*> passengers;

    auto maxPassengers = CAR_MAX_PASSENGERS(ref);

    for(int seatId = 0; seatId < maxPassengers; seatId++)
    {
        if(!CAR_PASSENGER_SEAT_FREE(ref, seatId))
        {
            auto passengerHandle = GET_ACTOR_HANDLE_FROM_CAR_PASSENGER_SEAT(ref, seatId);
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

bool Vehicle::IsPlayerInside()
{
    auto playerActor = GetPlayerActor();
    return IsPedInside(playerActor);
}

bool Vehicle::IsPedInside(int pedRef)
{
    auto occupants = GetCurrentOccupants();
    for(auto ped : occupants)
    {
        if(pedRef == ped->ref) return true;
    }
    return false;
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

int Vehicle::GetModelId()
{
    return GET_CAR_MODEL(ref);
}

void Vehicle::DestroyCarAndPeds()
{
    auto ocuppants = GetCurrentOccupants();

    for(auto ped : ocuppants)
    {
        DESTROY_ACTOR(ped->ref);
        Peds::RemovePed(ped->ref);
    }

    DESTROY_CAR(ref);
    Vehicles::RemoveVehicle(ref);
}

bool Vehicle::IsInChase()
{
    if(!Chase::InChase) return false;

    auto criminals = Criminals::GetCriminals();
    for(auto criminal : criminals)
    {
        if(IsPedInside(criminal->ref)) return true;
    }
    return false;
}