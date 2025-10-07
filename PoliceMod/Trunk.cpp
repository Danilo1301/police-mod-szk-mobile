#include "Trunk.h"

#include "CleoFunctions.h"
#include "Peds.h"
#include "BottomMessage.h"

Trunk::Trunk(int vehicleRef)
{
    this->vehicleRef = vehicleRef;
}

void Trunk::AddPedToTrunk(int pedRef)
{
    pedsInside.push_back(pedRef);

    CLEAR_ACTOR_TASK(pedRef);
    REMOVE_REFERENCES_TO_ACTOR(pedRef);

    auto offset = CVector(-0.50f, -1.80f, 0.90f);
    
    PUT_ACTOR_INTO_TURRET_ON_CAR(pedRef, vehicleRef, offset.x, offset.y, offset.z, 0, 0.0f, 0);

    Peds::GetPed(pedRef)->DoCover();

    BottomMessage::SetMessage("", "Colocado no porta-malas", 2000);
}

void Trunk::RemoveAllPeds()
{
    for(auto pedRef : pedsInside)
    {
        REMOVE_ACTOR_FROM_TURRET_MODE(pedRef);

        auto ped = Peds::GetPed(pedRef);

        ped->StopCover();
        ped->HideMapIcon();
    }
    pedsInside.clear();

    BottomMessage::SetMessage("", "Tirar do porta-malas", 2000);
}