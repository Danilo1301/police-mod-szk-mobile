#include "PoliceBase.h"

#include "CleoFunctions.h"
#include "Vehicles.h"
#include "BottomMessage.h"
#include "Peds.h"

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

        std::vector<Ped*> pedsEscorted;

        auto passengers = vehicle->GetCurrentPassengers();
        for(auto passengerRef : passengers)
        {
            auto ped = Peds::GetPed(passengerRef);

            if(ped->flags.beeingEscorted)
            {
                pedsEscorted.push_back(ped);
            }
        }

        if(pedsEscorted.size() == 0)
        {
            BottomMessage::SetMessage("~r~Voce nao esta transportando nenhum suspeito neste veiculo", 2000);
            return;
        }

        for(auto ped : pedsEscorted)
        {
            ped->LeaveCar();

            WAIT(3000, [ped]() {
                ped->QueueDestroy();
            });
        }

        if(pedsEscorted.size() == 1)
        {
            BottomMessage::SetMessage("O suspeito ficou a ~y~disposicao da justica", 3000);
        } else {
            BottomMessage::SetMessage("Os " + std::to_string(pedsEscorted.size()) + " suspeitos ficaram a ~y~disposicao da justica", 3000);
        }

        //BottomMessage::AddMessage("Recompensa: ~g~R$ 0", 3000);
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

void PoliceBase::OnPostDrawRadar()
{
    auto position = basePosition;
        
    menuSZK->DrawTextureOnRadar(texturePoliceDP, position, CRGBA(255, 255, 255), 50.0f);
}

void PoliceBase::OnDraw()
{
    auto position = basePosition;

    menuSZK->DrawTextureOnWorld(texturePoliceDP, position, CRGBA(255, 255, 255), CVector2D(100, 100));
}