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

        auto cped = (CPed*)carryingPed->ptr;

        auto matrix = cped->GetMatrix();

        matrix->pos = playerPosition;
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