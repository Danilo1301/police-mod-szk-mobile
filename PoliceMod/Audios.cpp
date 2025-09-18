#include "Audios.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

IAudio* Audios::audioEncostaCarro = NULL;
IAudio* Audios::audioDesceMaoCabeca = NULL;
IAudio* Audios::audioLiberado = NULL;
IAudio* Audios::audioPedirRG = NULL;

void Audios::CreateAudios()
{
    audioEncostaCarro = menuSZK->CreateAudio(getPathFromAssets("audios/officer/pull_over_car.wav"));
    audioDesceMaoCabeca = menuSZK->CreateAudio(getPathFromAssets("audios/officer/exit_vehicle_hands_up.wav"));
    audioLiberado = menuSZK->CreateAudio(getPathFromAssets("audios/officer/all_clear.wav"));
    audioPedirRG = menuSZK->CreateAudio(getPathFromAssets("audios/officer/ask_rg.wav"));
}

void Audios::Update()
{
    if(currentAudioGroup != NULL)
    {
        if(currentAudioGroup->HasCompleted())
        {
            currentAudioGroup->RemoveAndDestroyAudios();
            currentAudioGroup = NULL;
        }
    }
}

// --------------------------------

AudioGroup::AudioGroup()
{
    
}

void AudioGroup::AddAudio(IAudio* audio)
{
    audios.push_back(audio);
}

void AudioGroup::Play()
{
    PlayNext();
}

void AudioGroup::PlayNext()
{
    if (currentAudio >= audios.size())
    {
        currentAudio = 0;
        hasCompleted = true;
        return; // acabou a lista
    }

    auto first = audios[currentAudio];
    first->onFinish->Add([this]() {
        currentAudio++;
        PlayNext();
    });
    first->Play();
}

bool AudioGroup::HasCompleted()
{
    return hasCompleted;
}

void AudioGroup::RemoveAndDestroyAudios()
{
    for(auto audio : audios)
    {
        audio->Destroy();
    }
    audios.clear();
    currentAudio = 0;
}