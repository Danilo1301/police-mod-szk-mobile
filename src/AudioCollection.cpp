#include "AudioCollection.h"

#include "CleoFunctions.h"

std::map<std::string, AudioVariationGroup*> g_audioGroups;

IAudio* g_currentVoiceAudio = nullptr;

IAudio* AudioVariationGroup::PlayRandom()
{
    if (audios.empty())
        return nullptr;

    // Gera um índice aleatório válido
    int index = getRandomNumber(0, static_cast<int>(audios.size()) - 1);

    auto audio = audios[index];
    if (audio)
        audio->Play();

    return audio;
}

void AudioVariationGroup::LoadNewAudio(std::string src)
{
    auto audio = menuSZK->LoadAudio(src);

    if (!audio)
    {
        fileLog->Error("[AudioVariationGroup] Error loading audio: " + src);
        return;
    }


    audios.push_back(audio);
    fileLog->Error("[AudioVariationGroup] Registered audio: ");
}

void AudioCollection::Initialize()
{
    {
        auto group = audioPulloverCar = CreateGroup("pull_over_car");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/pull_over_car.wav"));
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer2/pull_over_car.wav"));
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
        auto group = audioExitVehicleHandsUp = CreateGroup("exit_vehicle_hands_up");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/exit_vehicle_hands_up.wav"));
    }

    {
        auto group = audioPlateCheck = CreateGroup("plate_check");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/officer/license_plate_check.wav"));
    }

    {
        auto group = audioInformSuspectRunning = CreateGroup("inform_suspect_running");
        group->LoadNewAudio(modData->GetFileFromAssets("audios/ai_officer/inform_suspect_running.mp3"));
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