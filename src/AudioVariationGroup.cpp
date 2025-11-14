#include "AudioVariationGroup.h"

IAudio* AudioVariationGroup::GetRandomAudio()
{
    if (audios.empty())
        return nullptr;

    // Gera um índice aleatório válido
    int index = getRandomNumber(0, static_cast<int>(audios.size()) - 1);

    auto audio = audios[index];
    
    return audio;
}

IAudio* AudioVariationGroup::PlayRandom()
{
    auto audio = GetRandomAudio();
    
    if (audio)
        audio->Play();

    return audio;
}

void AudioVariationGroup::LoadNewAudio(std::string src, bool in3d)
{
    auto audio = in3d ? menuSZK->Load3DAudio(src) : menuSZK->LoadAudio(src);

    if (!audio)
    {
        fileLog->Error("[AudioVariationGroup] Error loading audio: " + src);
        return;
    }

    audios.push_back(audio);
    fileLog->Error("[AudioVariationGroup] Registered audio: ");
}