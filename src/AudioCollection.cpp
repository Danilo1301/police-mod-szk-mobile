#include "AudioCollection.h"

#include "CleoFunctions.h"

std::map<std::string, AudioVariationGroup*> g_audioGroups;

IAudio* g_currentVoiceAudio = nullptr;

void AudioCollection::Initialize()
{
    {
        auto group = audioPulloverCar = CreateGroup("pull_over_car");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/pull_over_car.wav"));
        //group->LoadNewAudio(modData->GetFileFromAssets("audios/officer2/pull_over_car.wav"));
    }

    {
        auto group = audioFreePed = CreateGroup("free_ped");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/all_clear.wav"));
    }

    {
        auto group = audioAskRG = CreateGroup("ask_rg");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/ask_rg.wav"));
    }
    
    {
        auto group = audioAskCNH = CreateGroup("ask_cnh");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/ask_cnh.wav"));
    }

    {
        auto group = audioExitVehicleHandsUp = CreateGroup("exit_vehicle_hands_up");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/exit_vehicle_hands_up.wav"));
    }

    {
        auto group = audioPlateCheck = CreateGroup("plate_check");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/license_plate_check.wav"));
    }

    {
        auto group = audioRequestBackup = CreateGroup("request_backup");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/request_backup.wav"));
    }

    {
        auto group = audioCalloutATM = CreateGroup("callout_atm");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/callouts/atm.wav"));
    }

    {
        auto group = audioCalloutStolenCar = CreateGroup("callout_stolen_car");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/callouts/stolen_car.wav"));
    }

    {
        auto group = audioRequestTowTruck = CreateGroup("request_tow_truck");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/request_tow_truck.wav"));
    }
}

AudioVariationGroup* AudioCollection::CreateGroup(std::string key)
{
    auto group = new AudioVariationGroup();

    g_audioGroups[key] = group;

    return group;
}

AudioVariationGroup* AudioCollection::GetGroup(std::string key)
{
    auto it = g_audioGroups.find(key);
    if (it != g_audioGroups.end())
        return it->second;

    fileLog->Error("[AudioCollection] Group not find: " + key);
    return nullptr;
}

void AudioCollection::PlayAsVoice(AudioVariationGroup* group)
{
    if(g_currentVoiceAudio != nullptr)
    {
        //g_currentVoiceAudio->Stop();
    }

    auto audio = group->PlayRandom();

    g_currentVoiceAudio = audio;
}

void AudioCollection::PlayAsVoice(AudioVariationGroup* group, std::function<void()> callback)
{
    PlayAsVoice(group);

    WaitForAudioFinish(g_currentVoiceAudio, [callback]() {
        callback();
    });
}

void WaitForAudioFinish(IAudio* audio, std::function<void()> callback)
{
    CleoFunctions::AddWaitForFunction("wait_audio", [audio]() {

        if(!audio->Finished()) return false;

        return true;

    }, [callback]() {
        callback();
    });
}