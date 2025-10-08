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
#include "Globals.h"
#include "SpriteUtils.h"
#include "Scorch.h"

bool Vehicle::CanShowWidgetAnyTime = false;

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    isPoliceCar = IsVehicleModelAPoliceCar(GET_CAR_MODEL(ref));

    if(isPoliceCar)
    {
        SetMapIconColor(COLOR_POLICE);
    }

    plate = randomPlateLimited();
    chassis = gerarChassi();
    renavamCode = gerarRenavam();

    int anoAtual = getCurrentYear();
    
    manufactureYear = getRandomNumber(anoAtual - 20, anoAtual);
    modelYear = getRandomNumber(manufactureYear, manufactureYear + 1);
    
    outdatedDoc = calculateProbability(0.20f);

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

    trunk = new Trunk(ref);
}

Vehicle::~Vehicle()
{
    if(trunkCheckpoint)
    {
        Checkpoints::DestroyCheckpoint(trunkCheckpoint);
        trunkCheckpoint = nullptr;
    }
}

void Vehicle::Update()
{
    lifeTime = std::min(lifeTime + (int)menuSZK->deltaTime, 100000);
    timeSinceLastRepair = std::min(timeSinceLastRepair + (int)menuSZK->deltaTime, 100000);

    auto position = GetPosition();
    auto playerPosition = GetPlayerPosition();
    float distanceToPlayer = distanceBetweenPoints(playerPosition, position);

    bool isMenuOpen = PoliceMod::m_IsUsingMenu;

    bool canShowWidget = distanceToPlayer < 5.0f && Pullover::IsVehicleBeeingPulledOver(this);

    if(CanShowWidgetAnyTime)
        canShowWidget = distanceToPlayer < 5.0f;

    if (isMenuOpen)
        canShowWidget = false;

    if (!isWidgetVisible && canShowWidget)
    {
        // Cria o widget
        widgetOptions = menuSZK->CreateWidgetButton(
            500, 500,
            getPathFromMenuAssets("widget_background1.png"),
            getPathFromAssets("widget_frisk_car.png")
        );

        if (widgetOptions)
        {
            isWidgetVisible = true;

            widgetOptions->onClickWidget->Add([this]() {
                Pullover::OpenVehicleMenu(this);
            });
        }
    }
    else if (isWidgetVisible && !canShowWidget)
    {
        // Fecha o widget se não puder ser mostrado
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
    if(carHealth < 300 && timeSinceLastRepair > 1000)
    {
        timeSinceLastRepair = 0;
        SET_CAR_HEALTH(ref, 350);
        debug->AddLine("car health restored");
    }

    if(isPoliceCar)
    {
        if(Scorch::IsCarrying())
        {
            auto trunkPosition = GetCarPositionWithOffset(ref, trunkOffset);

            if(!trunkCheckpoint)
            {
                trunkCheckpoint = Checkpoints::CreateCheckpoint(trunkPosition);
            }
            
            trunkCheckpoint->position = trunkPosition;
            
            Scorch::TestCheckpoint(trunkCheckpoint, this);
        }

        if(!Scorch::IsCarrying() && trunkCheckpoint != nullptr)
        {
            Checkpoints::DestroyCheckpoint(trunkCheckpoint);
            trunkCheckpoint = nullptr;
        }
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

void Vehicle::SetMapIconColor(CRGBA color)
{
    mapIconColor = color;
}

void Vehicle::HideMapIcon()
{
    mapIconColor.a = 0;
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