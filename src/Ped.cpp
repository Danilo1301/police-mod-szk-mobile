#include "Ped.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Peds.h"
#include "Vehicles.h"
#include "WorldWidgets.h"
#include "Criminals.h"

#include "BottomMessage.h"

Ped::Ped(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    wasAlive = ACTOR_HEALTH(ref) > 0;

    UpdateSeatPosition();

    cpf = randomCPF();
    rg = randomRG();
    catHab = randomCatHab();

    if(calculateProbability(0.10))
    {
        catHab = "";
    }

    if(catHab.length() > 0)
        flags.expiredDriversLicense = calculateProbability(0.20);

    flags.wantedByJustice = calculateProbability(0.15);

    if(flags.wantedByJustice)
    {
        flags.willSurrender = false;
    } else {
        flags.willSurrender = calculateProbability(0.40);
    }
}

Ped::~Ped()
{
    if(widgetOptions)
    {
        widgetOptions->Close();
        widgetOptions = nullptr;
    }
}

void Ped::Update()
{
    PerformAnims();

    if(widgetOptions == nullptr)
    {
        auto widget = menuSZK->CreateWidget(
            200,
            200,
            100,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );
        widget->visible = false;

        WorldWidget w;
        w.widget = widget;
        w.attachToPed = ref;
        WorldWidgets.push_back(w);

        widget->onClick->Add([this]() {
            Pullover::OpenPedMenu(this);
        });

        widgetOptions = widget;
    }
    
    auto pedPosition = GetPosition();
    auto playerPosition = *g_playerPosition;

    if(widgetOptions)
    {
        bool widgetVisible = false;

        if(distanceBetweenPoints(pedPosition, playerPosition) < 5.0f)
        {
            widgetVisible = flags.showWidget;
        }

        widgetOptions->visible = widgetVisible;
    }

    auto health = ACTOR_HEALTH(ref);

    if(health <= 0 && wasAlive)
    {
        wasAlive = false;

        auto ref = this->ref;

        WAIT(6000, [ref]() {

            if(ref == GetPlayerActor()) return;

            if(!ACTOR_DEFINED(ref)) return;

            auto ped = Peds::GetPed(ref);
            auto modelId = GET_ACTOR_MODEL(ref);
            auto position = ped->GetPosition();

            auto newPedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, modelId, position.x, position.y, position.z);
            auto newPed = Peds::RegisterPed(newPedRef);

            newPed->CopyFrom(*ped);

            if(Criminals::IsCriminal(ped))
            {
                Criminals::AddCriminal(newPed);
                Criminals::RemoveCriminal(ped);
            }

            newPed->flags.isInconcious = true;
            newPed->flags.hasSurrended = false;
            newPed->flags.showWidget = false;
            newPed->flags.willSurrender = true;

            PERFORM_ANIMATION_AS_ACTOR(newPedRef, "crckdeth2", "CRACK", 10.0f, 0, 0, 0, 1, -1);

            DESTROY_ACTOR(ref);
            Peds::RemovePed(ref);

            auto cped = (CPed*)newPed->ptr;
            cped->m_matrix->at = CVector(position.x, position.y, position.z - 0.3f);
        });
    }

    if(isLeavingCar)
    {
        bool isIn = IsInAnyCar();

        if(!isIn)
        {
            isLeavingCar = false;
            menuDebug->AddLine("~y~Ped left the vehicle");
        }
    }

    if(isEnteringCar)
    {
        bool isIn = IsInAnyCar();

        if(isIn)
        {
            isEnteringCar = false;
            menuDebug->AddLine("~y~Ped entered the vehicle");
        }
    }

    if(flags.isInconcious)
    {
        auto distanceFromPlayer = distanceBetweenPoints(pedPosition, playerPosition);

        if(distanceFromPlayer > 200.0f)
        {
            DESTROY_ACTOR(ref);
            Peds::RemovePed(ref);
            return;
        }
    }
}

void Ped::SetCanDoHandsup()
{
    SetAnim("handsup", "PED");

    PerformAnims();
}

void Ped::PerformAnims()
{
    if(isLeavingCar) return;
    if(isEnteringCar) return;

    if(flags.isInconcious) return;

    if(currentAnim.length() == 0) return;

    if(!IsPerformingAnimation(currentAnim))
    {
        CLEAR_ACTOR_TASK(ref);
        PERFORM_ANIMATION_AS_ACTOR(ref, currentAnim.c_str(), currentAnimGroup.c_str(), 4.0f, 0, 0, 0, 1, -1);
    }
}

bool Ped::IsPerformingAnimation(const std::string& animName)
{
    return ACTOR_PERFORMING_ANIMATION(ref, animName.c_str());
}

void Ped::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Ped::HideBlip()
{
    flags.showBlip = false;
}

CVector Ped::GetPosition()
{
    return GetPedPosition(ref);
}

bool Ped::IsInAnyCar()
{
    return IS_CHAR_IN_ANY_CAR(ref);
}

int Ped::GetCurrentCar()
{
    return GetVehiclePedIsUsing(ref);
}

bool Ped::IsDriver()
{
    auto car = GetCurrentCar();

    if(car < 0) return false;

    return GET_DRIVER_OF_CAR(car) == ref;
}

void Ped::LeaveCar()
{
    if(!IsInAnyCar()) return;

    UpdateSeatPosition();
    isLeavingCar = true;

    EXIT_CAR_AS_ACTOR(ref);
}

void Ped::UpdateSeatPosition()
{
    previousVehicle = -1;

    if(!IsInAnyCar())
    {
        prevSeatPosition = SeatPosition::NONE;
        return;
    } 

    previousVehicle = GetCurrentCar();
    prevSeatPosition = IsDriver() ? SeatPosition::DRIVER : SeatPosition::PASSENGER;
}

void Ped::EnterVehicle(int vehicleRef, SeatPosition seat, int seatId)
{
    if(IsInAnyCar())
    {
        menuDebug->AddLine("~r~cant enter vehicle: already in one");
        return;
    }

    if(seat == SeatPosition::NONE)
    {
        menuDebug->AddLine("~r~cant enter vehicle: seat is NONE");
        return;
    }

    ClearAnim();
    CLEAR_ACTOR_TASK(ref);

    isEnteringCar = true;

    if(seat == SeatPosition::DRIVER)
    {
        ENTER_CAR_AS_DRIVER_AS_ACTOR(ref, vehicleRef, 10000);
    } else if(seat == SeatPosition::PASSENGER)
    {
        ACTOR_ENTER_CAR_PASSENGER_SEAT(ref, vehicleRef, 10000, seatId);
    }
}

void Ped::StartDrivingRandomly()
{
    if(!IsInAnyCar()) return;
    if(!IsDriver()) return;

    auto car = GetCurrentCar();

    REMOVE_REFERENCES_TO_CAR(car);
    SET_CAR_ENGINE_OPERATION(car, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(car, DrivingMode::StopForCars);
    SET_CAR_TO_PSYCHO_DRIVER(car);
    SET_CAR_MAX_SPEED(car, 20.0f);
}

void Ped::SetAnim(std::string anim, std::string animGroup)
{
    currentAnim = anim;
    currentAnimGroup = animGroup;
}

void Ped::ClearAnim()
{
    currentAnim = "";
    currentAnimGroup = "";
    ClearPedAnimations(ref);
}

void Ped::InitializeOnVehicle(int vehicleRef)
{
    auto vehicle = Vehicles::GetVehicle(vehicleRef);

    if(vehicle->flags.isStolen || vehicle->flags.hasExpiredDocument)
    {
        auto occupants = vehicle->GetCurrentOccupants();

        bool willSurrender = calculateProbability(0.40);

        for(auto pedRef : occupants)
        {
            auto ped = Peds::GetPed(pedRef);

            ped->flags.willSurrender = willSurrender;
        }
    }
}

void Ped::CopyFrom(const Ped& other)
{
    int oldRef = ref;
    void* oldPtr = ptr;

    *this = other; // reutiliza o operador=

    ref = oldRef;
    ptr = oldPtr;
    widgetOptions = nullptr;

    if (vehicleOwned > 0)
    {
        auto vehicle = Vehicles::GetVehicle(vehicleOwned);

        if (!vehicle)
            return;

        // 🚮 Remove o ped antigo da lista de passageiros (se existir)
        auto& passengers = vehicle->ownerPassengers;
        passengers.erase(
            std::remove(passengers.begin(), passengers.end(), oldRef),
            passengers.end()
        );

        // 🚗 Atualiza o novo dono conforme a posição no veículo
        if (prevSeatPosition == SeatPosition::DRIVER)
        {
            vehicle->ownerDriver = ref;
        }
        else if (prevSeatPosition == SeatPosition::PASSENGER)
        {
            passengers.push_back(ref);
        }
    }
}

void Ped::Reanimate()
{
    ClearAnim();
    flags.isInconcious = false;
    
    if(Criminals::IsCriminal(this))
    {
        flags.hasSurrended = true;
        SetCanDoHandsup();
    } else {
        HideBlip();
        flags.showWidget = false;
    }
}

void Ped::DestroySelf()
{
    if(!ACTOR_DEFINED(ref)) return;

    DESTROY_ACTOR(ref);
    Peds::RemovePed(ref);
}