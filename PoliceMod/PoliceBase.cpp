#include "PoliceBase.h"

#include "SpriteUtils.h"
#include "CleoFunctions.h"
#include "Vehicles.h"

void PoliceBase::Update()
{
    auto playerActor = GetPlayerActor();

    auto vehicleRef = GetVehiclePedIsUsing(playerActor);

    if(vehicleRef > 0)
    {   
        auto vehicle = Vehicles::GetVehicle(vehicleRef);

        auto carPosition = GetCarPosition(vehicleRef);

        if(leaveCriminalCheckpoint->IsInRange(carPosition))
        {
            if(vehicle->trunk->HasPedsInside())
            {
                vehicle->trunk->RemoveAllPeds();
            }
        }
    }
}

void PoliceBase::OnRender()
{
    // em cima do checkpoint
    SpriteUtils::DrawSpriteInWorld(SpriteUtils::spritePoliceDepartment, basePosition, 100.0f, CRGBA(0, 0, 255));
}

void PoliceBase::OnDrawRadar()
{
    auto distanceFromPlayer = DistanceFromPed(GetPlayerActor(), basePosition);

    if(!IsGamePaused())
    {
        if(distanceFromPlayer > 300) return;
    }

    // icone no mapa
    SpriteUtils::DrawSpriteInRadar(SpriteUtils::spritePoliceDepartment, basePosition, CRGBA(0, 0, 255), 50.0f);
}