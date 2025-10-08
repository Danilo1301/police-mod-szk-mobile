#include "Scorch.h"

#include "CleoFunctions.h"
#include "BottomMessage.h"
#include "Pullover.h"

Ped* carryingPed = nullptr;

void Scorch::Update()
{
    if(carryingPed)
    {
        auto playerPosition = GetPedPositionWithOffset(GetPlayerActor(), CVector(0, 1.1f, 0));
        
        carryingPed->SetPosition(playerPosition);
    }
}

void Scorch::StartCarry(Ped* ped)
{
    if(IsCarrying()) StopCarry();

    carryingPed = ped;
}

void Scorch::StopCarry()
{
    carryingPed = nullptr;
}

void Scorch::TestCheckpoint(Checkpoint* checkpoint, Vehicle* vehicle)
{
    auto ped = Scorch::GetCarryingPed();
    auto pedPosition = ped->GetPosition();

    if(checkpoint->IsInRange(pedPosition))
    {
        logInternal("Entered checkpoint, now stop carry and put in trunk");

        Scorch::StopCarry();

        Pullover::RemovePedFromPullover(ped);

        vehicle->trunk->AddPedToTrunk(ped->ref);
    }
}

bool Scorch::IsCarrying()
{
    return carryingPed != nullptr;
}

Ped* Scorch::GetCarryingPed()
{
    return carryingPed;
}