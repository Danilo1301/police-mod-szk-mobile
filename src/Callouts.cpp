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
#include "AudioCollection.h"
#include "RadioSounds.h"

#include "callouts/ATMCallout.h"

bool g_onACallout = false;
bool g_calloutReached = false;
int g_receiveCalloutTimer = 0;

static CalloutBase* g_broadcastingCallout = nullptr;
static CalloutBase* g_currentCallout = nullptr;

bool g_spawnedCriminals = false;

void Callouts::Initialize()
{
    CalloutRegistry::Register(new ATMCallout());
}

void Callouts::Update()
{
    if(g_broadcastingCallout == nullptr && !g_onACallout)
    {
        g_receiveCalloutTimer += menuSZK->deltaTime;
    }

    if(!g_onACallout)
    {
        g_spawnedCriminals = false;
        g_calloutReached = false;
    } else {

        auto criminalsCount = Criminals::GetCriminals()->size();

        if(criminalsCount > 0 && g_calloutReached)
        {
            g_spawnedCriminals = true;
        }

        if(g_spawnedCriminals && criminalsCount == 0 && g_calloutReached)
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

void Callouts::TryBroadcastCallout()
{
    if(g_receiveCalloutTimer >= 10000)
    {
        BroadcastRandomCallout();
    }
}

void Callouts::BroadcastRandomCallout()
{
    g_receiveCalloutTimer = 0;
    g_broadcastingCallout = CalloutRegistry::GetRandom();

    if (!g_broadcastingCallout) return;

    BottomMessage::SetMessage(g_broadcastingCallout->GetBroadcastMessage(), 8000);

    auto audioGroup = g_broadcastingCallout->GetBroadcastAudio();
    auto audio = audioGroup->GetRandomAudio();
    
    RadioSounds::PlayAudioNow(audio);

    WaitForAudioFinish(audio, []() {
        WAIT(1000, []() {
            g_broadcastingCallout = nullptr;
        });
    });
}

void Callouts::AcceptCallout()
{
    if(g_broadcastingCallout == nullptr) return;
   
    g_onACallout = true;

    g_onACallout = true;
    g_currentCallout = g_broadcastingCallout;
    g_broadcastingCallout = nullptr;

    g_currentCallout->OnAccept();
}

// void Callouts::OnBeginCallout(int id)
// {
//     if(id == CALLOUT_ATM)
//     {
//         BottomMessage::SetMessage("~w~Desloque-se ate o ~y~local ~w~no mapa", 3000);

//         auto location = ATMSystem::GetRandomLocation();
//         auto position = location->position;

//         int marker = CreateMarker(position.x, position.y, position.z, 0, 3, 3);

//         ScriptTask* taskAproach = new ScriptTask("aproach_callout");
//         taskAproach->onBegin = []() {

//         };
//         taskAproach->onExecute = [location]() {

//             auto playerPosition = GetPlayerPosition();
//             auto distance = distanceBetweenPoints(playerPosition, location->position);
         
//             if(distance > 150.0f) return SCRIPT_KEEP_GOING;

//             return SCRIPT_SUCCESS;
//         };
//         taskAproach->onComplete = [marker, location]() {
            
//             g_calloutReached = true;

//             DISABLE_MARKER(marker);

//             BottomMessage::SetMessage("~r~Detenha os suspeitos", 4000);

//             TryCreateExplosion(location->position);

//             std::vector<int> criminalSkins;
//             for(int i = 0; i < 4; i++)
//             {
//                 auto skin = GetRandomCriminalSkin();

//                 criminalSkins.push_back(skin);
//                 ModelLoader::AddModelToLoad(skin);
//             }

//             int coltId = 22;
//             int coltModel = 346;

//             ModelLoader::AddModelToLoad(coltModel);
//             ModelLoader::LoadAll([location, criminalSkins, coltId]() {
//                 for(int i = 0; i < 4; i++)
//                 {
//                     auto skinModel = criminalSkins[i];

//                     auto pedRef = SpawnPedRandomlyAtPosition_PedNode(location->position, PedType::CivMale, skinModel, 10.0f);
//                     auto criminal = Peds::RegisterPed(pedRef);

//                     Criminals::AddCriminal(criminal);

//                     criminal->ShowBlip(COLOR_CRIMINAL);

//                     GIVE_ACTOR_WEAPON(pedRef, coltId, 1000);
//                     SET_ACTOR_HEALTH(criminal->ref, 500);

//                     criminal->flags.willSurrender = false;

//                     if(i >= 1)
//                     {
//                         criminal->flags.willKillCops = true;
//                     } else {
//                         criminal->flags.willKillCops = false;
//                     }
//                 }
//             });
//         };
//         taskAproach->Start();
//     }
// }

bool Callouts::HasCalloutToAccept()
{
    return g_broadcastingCallout != nullptr;
}

bool Callouts::IsBroacastingCallout()
{
    return g_broadcastingCallout != nullptr;
}

bool Callouts::IsInCallout()
{
    return g_onACallout;
}