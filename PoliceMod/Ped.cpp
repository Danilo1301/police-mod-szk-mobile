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

    if(DialogManager::loadedDialogs.size() > 0)
    {
        int idx = getRandomNumber(0, DialogManager::loadedDialogs.size() - 1);
    
        dialogue = DialogManager::loadedDialogs[idx];
    }

    auto currentYear = getCurrentYear();

    birthDate = randomBirthDate(1970, 2000);
    cpf = randomCPF();
    rg = randomRG();
    catHab = gerarCatHab();
    cnhExpireDate = gerarValidadeCNH(currentYear - 3, currentYear + 10);

    if(!dialogue.variables.hasCNH)
    {
        catHab = "";
    }

    inventory.AddItem(new InventoryItem("docs"));

    inVehicleState = IsInAnyCar() ? PedInVehicleState::SITTING : PedInVehicleState::NOT_ON_VEHICLE;
}

Ped::~Ped()
{

}

void Ped::Update()
{
    justLeftVehicle = false;

    if (!IsInAnyCar() && inVehicleState != PedInVehicleState::NOT_ON_VEHICLE)
    {
        inVehicleState = PedInVehicleState::NOT_ON_VEHICLE;
        justLeftVehicle = true;
        debug->AddLine("ped just left vehicle");
    }
    else if (IsInAnyCar() && inVehicleState != PedInVehicleState::SITTING)
    {
        inVehicleState = PedInVehicleState::SITTING;
        // justEnteredVehicle = true;
        debug->AddLine("ped just entered vehicle");
    }

    auto position = GetPosition();
    auto playerPosition = GetPlayerPosition();
    float distanceToPlayer = distanceBetweenPoints(playerPosition, position);

    if (doHandsup)
    {
        if (!IsDoingHandsupAnim() && !IsInAnyCar())
        {
            CLEAR_ACTOR_TASK(ref);
            PERFORM_ANIMATION_AS_ACTOR(
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
}

void Ped::DoHandsup()
{
    doHandsup = true;

    if(!IsInAnyCar())
    {
        CLEAR_ACTOR_TASK(ref);
        PERFORM_ANIMATION_AS_ACTOR(ref, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
    }
}

void Ped::StopHandsup()
{
    doHandsup = false;

    CLEAR_ACTOR_TASK(ref);
    ClearPedAnimations(ref);
}

bool Ped::IsDoingHandsupAnim()
{
    bool isDoingHandsup = ACTOR_PERFORMING_ANIMATION(ref, "handsup");
    return isDoingHandsup;
}

bool Ped::IsInAnyCar()
{
    return IS_CHAR_IN_ANY_CAR(ref);
}

CVector Ped::GetPosition()
{
    return GetPedPosition(ref);
}

void Ped::StartDrivingRandomly()
{
    if(!IS_CHAR_IN_ANY_CAR(ref))
    {
        debug->AddLine("~r~ped is not in any car");
        return;
    }
    
    auto vehicleRef = ACTOR_USED_CAR(ref);

    REMOVE_REFERENCES_TO_CAR(vehicleRef);
    SET_CAR_ENGINE_OPERATION(vehicleRef, true);
    SET_CAR_TRAFFIC_BEHAVIOUR(vehicleRef, DrivingMode::StopForCars);
    SET_CAR_TO_PSYCHO_DRIVER(vehicleRef);
    SET_CAR_MAX_SPEED(vehicleRef, 20.0f);
}

void Ped::LeaveCar()
{
    if(!IsInAnyCar())
    {
        debug->AddLine("~r~ped is not in a car");
        return;
    }

    auto carRef = ACTOR_USED_CAR(ref);
    auto driverRef = GET_DRIVER_OF_CAR(carRef);

    if(driverRef == ref)
    {
        previousSeat = PedSeat::DRIVER;
    } else {
        previousSeat = PedSeat::PASSENGER;
    }

    inVehicleState = PedInVehicleState::LEAVING;

    EXIT_CAR_AS_ACTOR(ref);
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

    CLEAR_ACTOR_TASK(ref);

    inVehicleState = PedInVehicleState::ENTERING;

    if(seat == PedSeat::DRIVER)
    {
        ENTER_CAR_AS_DRIVER_AS_ACTOR(ref, vehicleRef, 10000);
    } else if(seat == PedSeat::PASSENGER)
    {
        ACTOR_ENTER_CAR_PASSENGER_SEAT(ref, vehicleRef, 10000, seatId);
    }
}

int Ped::AddBlip()
{
    if(blip != NO_BLIP) RemoveBlip();
    blip = ADD_BLIP_FOR_CHAR(ref);
    return blip;
}

void Ped::RemoveBlip()
{
    if(blip == NO_BLIP) return;

    DISABLE_MARKER(blip);
    blip = NO_BLIP;
}