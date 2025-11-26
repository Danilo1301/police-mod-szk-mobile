#include "DrugDealerCallout.h"

#include "../AudioCollection.h"
#include "../BottomMessage.h"
#include "../CleoFunctions.h"
#include "../ScriptTask.h"
#include "../ModelLoader.h"
#include "../Biqueiras.h"
#include "../Peds.h"
#include "../InventoryItemManager.h"

extern bool g_calloutReached;

const char* DrugDealerCallout::GetBroadcastMessage()
{
    return "~y~[COPOM] ~w~Um veiculo roubado foi localizado pelas cameras do olho vivo";
}

AudioVariationGroup* DrugDealerCallout::GetBroadcastAudio()
{
    return audioCalloutStolenCar;
}

void DrugDealerCallout::OnAccept()
{
    BottomMessage::SetMessage("~w~Desloque-se ate o ~y~local da denuncia ~w~no mapa", 3000);

    auto location = Biqueiras::GetRandomCloseBiqueira(GetPlayerPosition());

    int marker = CreateMarker(location.position.x, location.position.y, location.position.z, 0, 3, 3);

    ScriptTask* taskAproach = new ScriptTask("aproach_callout");
    taskAproach->onBegin = []() {

    };
    taskAproach->onExecute = [location]() {

        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, location.position);
        
        if(distance > 100.0f) return SCRIPT_KEEP_GOING;

        return SCRIPT_SUCCESS;
    };
    taskAproach->onComplete = [marker, location]() {
        
        g_calloutReached = true;

        DISABLE_MARKER(marker);

        BottomMessage::SetMessage("~r~Aborde o suspeito", 4000);

        auto skin = GetRandomCriminalSkin();

        ModelLoader::AddModelToLoad(skin);
        ModelLoader::LoadAll([skin, location]() {

            auto dealerRef = CREATE_ACTOR_PEDTYPE(PedType::Special, skin, location.position.x, location.position.y, location.position  .z);
            auto dealer = Peds::RegisterPed(dealerRef);

            dealer->ShowBlip(COLOR_CRIMINAL);

            dealer->flags.willSurrender = calculateProbability(0.20);

            if(dealer->flags.willKillCops == false)
            {
                dealer->flags.willKillCops = calculateProbability(0.20);
            }

            for(auto item : InventoryItemManager::itemDefinitions)
            {
                auto id = item.first;
                auto itemDef = *item.second;

                if(!ItemDefinitionContainFlag(itemDef, "used_by_drug_dealer"))
                    continue;

                if(calculateProbability(itemDef.chance))
                {
                    int amount = getRandomNumber(1, itemDef.maxAmount);

                    dealer->inventory.AddItem(id, amount);
                }
            }

        });

    };
    taskAproach->Start();
}