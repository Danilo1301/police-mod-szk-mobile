#include "StolenCarCallout.h"

#include "../AudioCollection.h"
#include "../RadioSounds.h"
#include "../BottomMessage.h"
#include "../ScriptTask.h"
#include "../CleoFunctions.h"
#include "../ATMSystem.h"
#include "../ModelLoader.h"
#include "../Peds.h"
#include "../Criminals.h"
#include "../Callouts.h"
#include "../Vehicles.h"

extern bool g_calloutReached;

const char* StolenCarCallout::GetBroadcastMessage()
{
    return "~y~[COPOM] ~w~Um veiculo roubado foi identificado pelas cameras";
}

AudioVariationGroup* StolenCarCallout::GetBroadcastAudio()
{
    return audioCalloutStolenCar;
}

void StolenCarCallout::OnAccept()
{
    BottomMessage::SetMessage("~w~Desloque-se ate o ~y~local do veiculo ~w~no mapa", 3000);

    const float CALLOUT_DISTANCE = 200.0f;

    auto playerActor = GET_PLAYER_ACTOR(0);
    auto distX = getRandomNumber(-CALLOUT_DISTANCE, CALLOUT_DISTANCE);

    float x = 0, y = 0, z = 0;
    STORE_COORDS_FROM_ACTOR_WITH_OFFSET(playerActor, (float)distX, CALLOUT_DISTANCE, 0, &x, &y, &z);

    auto carNodePosition = GET_CLOSEST_CAR_NODE(x, y, z);

    int marker = CreateMarker(carNodePosition.x, carNodePosition.y, carNodePosition.z, 0, 3, 3);

    ScriptTask* taskAproach = new ScriptTask("aproach_callout");
    taskAproach->onBegin = []() {

    };
    taskAproach->onExecute = [carNodePosition]() {

        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, carNodePosition);
        
        if(distance > 150.0f) return SCRIPT_KEEP_GOING;

        return SCRIPT_SUCCESS;
    };
    taskAproach->onComplete = [marker, carNodePosition]() {
        
        g_calloutReached = true;

        DISABLE_MARKER(marker);

        BottomMessage::SetMessage("~r~Aborde o veiculo suspeito", 4000);

        std::vector<int> vehicleIds;
        for(auto id : g_stolenVehicleIds) vehicleIds.push_back(id);

        int vehicleModel = vehicleIds[getRandomNumber(0, vehicleIds.size() -1)];
        int driverModel = 20;

        ModelLoader::AddModelToLoad(vehicleModel);
        ModelLoader::AddModelToLoad(driverModel);
        ModelLoader::LoadAll([vehicleModel, driverModel, carNodePosition]() {

            int carRef = CREATE_CAR_AT(vehicleModel, carNodePosition.x, carNodePosition.y, carNodePosition.z);

            auto car = Vehicles::RegisterVehicle(carRef);
            car->flags.isStolen = true;
            car->ShowBlip(COLOR_CRIMINAL);

            auto driverRef = CREATE_ACTOR_PEDTYPE_IN_CAR_DRIVERSEAT(carRef, PedType::CivMale, driverModel);
            auto driver = Peds::RegisterPed(driverRef);

            Criminals::AddCriminal(driver);

            REMOVE_REFERENCES_TO_CAR(carRef);
            SET_CAR_ENGINE_OPERATION(carRef, true);
            SET_CAR_TO_PSYCHO_DRIVER(carRef);
        });
    };
    taskAproach->Start();
}