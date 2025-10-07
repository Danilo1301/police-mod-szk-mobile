#include "PoliceBase.h"

#include "SpriteUtils.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "BottomMessage.h"

PoliceBase::PoliceBase()
{
    leaveCriminalCheckpoint = Checkpoints::CreateCheckpoint(CVector(0, 0, 0));

    leaveCriminalCheckpoint->onEnterCheckpoint = []() {
        auto playerActor = GetPlayerActor();

        auto vehicleRef = GetVehiclePedIsUsing(playerActor);

        if(vehicleRef <= 0)
        {
            BottomMessage::SetMessage("~r~Voce nao esta em um veiculo", 2000);
            return;
        }

        auto vehicle = Vehicles::GetVehicle(vehicleRef);

        if(vehicle->trunk->HasPedsInside())
        {
            vehicle->trunk->RemoveAllPeds();
        } else {
            BottomMessage::SetMessage("~r~Voce nao tem suspeitos no porta malas", 2000);
        }
    };
}

void PoliceBase::Update()
{
    auto playerActor = GetPlayerActor();
    auto vehicleRef = GetVehiclePedIsUsing(playerActor);

    if(vehicleRef > 0)
    {   
        leaveCriminalCheckpoint->CheckEntered(GetCarPosition(vehicleRef));
    } else {
        leaveCriminalCheckpoint->CheckEntered(GetPlayerPosition());
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