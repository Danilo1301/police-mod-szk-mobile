#include "Callouts.h"

#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "ATMSystem.h"
#include "ScriptTask.h"
#include "ModelLoader.h"
#include "Peds.h"
#include "Criminals.h"

bool g_onACallout = false;
int g_timeToGetCallout = 30000;
int g_broadcastingCalloutId = NO_CALLOUT;
int g_previousCalloutId = NO_CALLOUT;

void TryCreateExplosion(CVector position)
{
    ScriptTask* task = new ScriptTask("explosion");
    task->onBegin = []() {
    };
    task->onExecute = [position]() {
        auto playerPosition = GetPlayerPosition();
        auto distance = distanceBetweenPoints(playerPosition, position);
        
        if(!g_onACallout) return SCRIPT_CANCEL;
        if(distance > 60.0f) return SCRIPT_KEEP_GOING;

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
        g_timeToGetCallout += menuSZK->deltaTime;

        if(g_timeToGetCallout >= 40000)
        {
            BroadcastRandomCallout();
        }
    }

    if(g_onACallout)
    {
        if(ACTOR_DEAD(GetPlayerActor()))
        {
            g_onACallout = false;

            BottomMessage::SetMessage("~r~Parece q o player morreu...", 2000);

            auto criminals = *Criminals::GetCriminals();

            for(auto criminal : criminals)
            {
                Criminals::RemoveCriminal(criminal);
                criminal->DestroySelf();
            }
        }
    }
}

void Callouts::BroadcastRandomCallout()
{
    g_timeToGetCallout = 0;
    g_broadcastingCalloutId = CALLOUT_ATM;
    g_previousCalloutId = g_broadcastingCalloutId;

    int time = 3000;

    BottomMessage::SetMessage("~g~[COPOM] ~w~Ocorrencia de roubo de caixa eletronico", time);
    
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

            BottomMessage::SetMessage("~r~Elimine os suspeitos", 4000);

            TryCreateExplosion(location->position);

            int skinModel = 29;
            int coltId = 22;
            int coltModel = 346;

            ModelLoader::AddModelToLoad(skinModel);
            ModelLoader::AddModelToLoad(coltModel);
            ModelLoader::LoadAll([location, skinModel, coltId]() {
                for(int i = 0; i < 4; i++)
                {
                    auto pedRef = SpawnPedRandomlyAtPosition_PedNode(location->position, PedType::CivMale, skinModel, 10.0f);
                    auto criminal = Peds::RegisterPed(pedRef);

                    Criminals::AddCriminal(criminal);

                    criminal->ShowBlip(COLOR_CRIMINAL);

                    GIVE_ACTOR_WEAPON(pedRef, coltId, 1000);
                    SET_ACTOR_HEALTH(criminal->ref, 500);

                    KILL_ACTOR(criminal->ref, GetPlayerActor());
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