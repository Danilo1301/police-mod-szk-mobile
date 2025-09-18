#include "Ped.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"
#include "Pullover.h"
#include "PoliceMod.h"

Ped::Ped(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    birthDate = randomBirthDate(1970, 2000);
    cpf = randomCPF();
    rg = randomRG();
}

Ped::~Ped()
{

}

void Ped::Update()
{
    auto position = GetPosition();
    auto playerPosition = GetPlayerPosition();
    float distanceToPlayer = distanceBetweenPoints(playerPosition, position);

    if (doHandsup)
    {
        if (!IsDoingHandsupAnim() && !IsInAnyCar())
        {
            CleoFunctions::CLEAR_ACTOR_TASK(ref);
            CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(
                ref, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1
            );
        }
    }

    auto isMenuOpen = PoliceMod::m_IsUsingMenu;

    if (!isWidgetVisible && doHandsup && distanceToPlayer < 3.0f && !isMenuOpen)
    {
        widgetOptions = menuSZK->CreateWidgetButton(
            500, 500,
            getPathFromMenuAssets("widget_background1.png"),
            getPathFromAssets("widget_frisk.png")
        );
        isWidgetVisible = true;

        widgetOptions->onClickWidget->Add([this]() {
            Pullover::OpenPedMenu(this);
        });
    }
    else if (isWidgetVisible && (!doHandsup || distanceToPlayer > 4.0f || isMenuOpen))
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

void Ped::DoHandsup()
{
    doHandsup = true;

    if(!IsInAnyCar())
    {
        CleoFunctions::CLEAR_ACTOR_TASK(ref);
        CleoFunctions::PERFORM_ANIMATION_AS_ACTOR(ref, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
    }
}

void Ped::StopHandsup()
{
    doHandsup = false;

    CleoFunctions::CLEAR_ACTOR_TASK(ref);
    ClearPedAnimations(ref);
}

bool Ped::IsDoingHandsupAnim()
{
    bool isDoingHandsup = CleoFunctions::ACTOR_PERFORMING_ANIMATION(ref, "handsup");
    return isDoingHandsup;
}

bool Ped::IsInAnyCar()
{
    return CleoFunctions::IS_CHAR_IN_ANY_CAR(ref);
}

CVector Ped::GetPosition()
{
    return GetPedPosition(ref);
}

void Ped::StartDrivingRandomly()
{
    if(!CleoFunctions::IS_CHAR_IN_ANY_CAR(ref))
    {
        debug->AddLine("~r~ped is not in any car");
        return;
    }
    
    auto vehicleRef = CleoFunctions::ACTOR_USED_CAR(ref);

    CleoFunctions::REMOVE_REFERENCES_TO_CAR(vehicleRef);
    CleoFunctions::SET_CAR_ENGINE_OPERATION(vehicleRef, true);
    CleoFunctions::SET_CAR_TRAFFIC_BEHAVIOUR(vehicleRef, 0);
    CleoFunctions::SET_CAR_TO_PSYCHO_DRIVER(vehicleRef);
    CleoFunctions::SET_CAR_MAX_SPEED(vehicleRef, 30.0f);
}

void Ped::LeaveCar()
{
    if(!IsInAnyCar())
    {
        debug->AddLine("~r~ped is not in a car");
        return;
    }

    auto carRef = CleoFunctions::ACTOR_USED_CAR(ref);
    auto driverRef = CleoFunctions::GET_DRIVER_OF_CAR(carRef);

    if(driverRef == ref)
    {
        previousSeat = PedSeat::DRIVER;
    } else {
        previousSeat = PedSeat::PASSENGER;
    }

    CleoFunctions::EXIT_CAR_AS_ACTOR(ref);
}

void Ped::EnterVehicle(int vehicleRef, PedSeat seat, int seatId)
{
    if(IsInAnyCar())
    {
        debug->AddLine("~r~cant enter vehicle: already in one");
        return;
    }

    if(seat == PedSeat::NO_SEAT)
    {
        debug->AddLine("~r~cant enter vehicle: seat is NO_SEAT");
        return;
    }

    CleoFunctions::CLEAR_ACTOR_TASK(ref);

    if(seat == PedSeat::DRIVER)
    {
        CleoFunctions::ENTER_CAR_AS_DRIVER_AS_ACTOR(ref, vehicleRef, 10000);
    } else if(seat == PedSeat::PASSENGER)
    {
        CleoFunctions::ACTOR_ENTER_CAR_PASSENGER_SEAT(ref, vehicleRef, 10000, seatId);
    }
}

int Ped::AddBlip()
{
    if(blip != NO_BLIP) RemoveBlip();
    blip = CleoFunctions::ADD_BLIP_FOR_CHAR(ref);
    return blip;
}

void Ped::RemoveBlip()
{
    if(blip == NO_BLIP) return;

    CleoFunctions::DISABLE_MARKER(blip);
    blip = NO_BLIP;
}