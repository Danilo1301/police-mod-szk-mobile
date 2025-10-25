#include "Callouts.h"

#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "ATMSystem.h"
#include "ScriptTask.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "Criminals.h"
#include "AIController.h"
#include "AICriminal.h"

bool g_onACallout = false;
int g_receiveCalloutTimer = 0;
int g_broadcastingCalloutId = NO_CALLOUT;
int g_previousCalloutId = NO_CALLOUT;

bool g_spawnedCriminals = false;

void TryCreateExplosion(CVector position)
{
    ScriptTask* task = new ScriptTask("explosion");
    task->onBegin = []() {
    };
    task->onExecute = [position]() {
        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, position);
        
        if(!g_onACallout) return SCRIPT_CANCEL;
        if(distance > 50.0f) return SCRIPT_KEEP_GOING;

        return SCRIPT_SUCCESS;
    };
    task->onComplete = [position]() {
        ADD_EXPLOSION(position.x, position.y, position.z, 0);
    };
    task->Start();
}

void Callouts::Update()
{
    if(g_broadcastingCalloutId == NO_CALLOUT && !g_onACallout)
    {
        g_receiveCalloutTimer += menuSZK->deltaTime;

        if(g_receiveCalloutTimer >= 40000)
        {
            BroadcastRandomCallout();
        }
    }

    if(!g_onACallout)
    {
        g_spawnedCriminals = false;
    } else {

        auto criminalsCount = Criminals::GetCriminals()->size();

        if(criminalsCount > 0)
        {
            g_spawnedCriminals = true;
        }

        if(g_spawnedCriminals && criminalsCount == 0)
        {
            g_onACallout = false;
            BottomMessage::AddMessage("Ocorrencia encerrada", 3000);
        }
    }

    if(g_onACallout)
    {
        if(ACTOR_DEAD(GetPlayerActor()))
        {
            g_onACallout = false;

            BottomMessage::SetMessage("~r~Parece q o player morreu...", 2000);

            WAIT(500, []() {
                auto criminals = *Criminals::GetCriminals();

                for(auto criminal : criminals)
                {
                    Criminals::RemoveCriminal(criminal);
                    criminal->QueueDestroy();
                }
            });
        }
    }
}

void Callouts::BroadcastRandomCallout()
{
    g_receiveCalloutTimer = 0;
    g_broadcastingCalloutId = CALLOUT_ATM;
    g_previousCalloutId = g_broadcastingCalloutId;

    int time = 6000;

    BottomMessage::SetMessage("~y~[COPOM] ~w~Ocorrencia de roubo de caixa eletronico em andamento", time);
    
    WAIT(time, []() {
        g_broadcastingCalloutId = NO_CALLOUT;
    });
}

void Callouts::AcceptCallout()
{
    if(g_previousCalloutId == NO_CALLOUT) return;
    auto id = g_previousCalloutId;
    g_previousCalloutId = NO_CALLOUT;

    g_onACallout = true;

    OnBeginCallout(id);
}

void Callouts::OnBeginCallout(int id)
{
    if(id == CALLOUT_ATM)
    {
        BottomMessage::SetMessage("~w~Desloque-se ate o ~r~local ~w~no mapa", 3000);

        auto location = ATMSystem::GetRandomLocation();
        auto position = location->position;

        int marker = CreateMarker(position.x, position.y, position.z, 0, 3, 3);

        ScriptTask* taskAproach = new ScriptTask("aproach_callout");
        taskAproach->onBegin = []() {

        };
        taskAproach->onExecute = [location]() {

            auto playerPosition = GetPlayerPosition();
            auto distance = distanceBetweenPoints(playerPosition, location->position);
         
            if(distance > 150.0f) return SCRIPT_KEEP_GOING;

            return SCRIPT_SUCCESS;
        };
        taskAproach->onComplete = [marker, location]() {
            
            DISABLE_MARKER(marker);

            BottomMessage::SetMessage("~r~Detenha os suspeitos", 4000);

            TryCreateExplosion(location->position);

            int skinModel = 29;
            int coltId = 22;
            int coltModel = 346;

            ModelLoader::AddModelToLoad(skinModel);
            ModelLoader::AddModelToLoad(coltModel);
            ModelLoader::LoadAll([location, skinModel, coltId]() {
                for(int i = 1; i <= 4; i++)
                {
                    auto pedRef = SpawnPedRandomlyAtPosition_PedNode(location->position, PedType::CivMale, skinModel, 10.0f);
                    auto criminal = Peds::RegisterPed(pedRef);

                    Criminals::AddCriminal(criminal);

                    criminal->ShowBlip(COLOR_CRIMINAL);

                    GIVE_ACTOR_WEAPON(pedRef, coltId, 1000);
                    SET_ACTOR_HEALTH(criminal->ref, 500);

                    criminal->flags.willSurrender = false;

                    if(i >= 2)
                    {
                        criminal->flags.willKillCops = true;
                    } else {
                        criminal->flags.willKillCops = false;
                    }
                }
            });
        };
        taskAproach->Start();
    }
}

bool Callouts::HasCalloutToAccept()
{
    return g_previousCalloutId != NO_CALLOUT;
}