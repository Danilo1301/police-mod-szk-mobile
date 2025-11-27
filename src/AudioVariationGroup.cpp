#include "AudioVariationGroup.h"

std::vector<std::string> GetSoundVariants(const std::string& fullPath)
{
    namespace fs = std::filesystem;
    std::vector<std::string> sounds;

    fileLog->Log("[Variants] Checking file: " + fullPath);

    fs::path path(fullPath);

    if (!fs::exists(path))
    {
        fileLog->Error("[Variants] ❌ Base file NOT FOUND");
        return sounds; // não tem nem o primeiro, já retorna
    }

    // Remove extensão para poder gerar pull_over_vehicle2.wav, 3.wav...
    std::string baseName = path.replace_extension().string();
    std::string ext = ".wav"; // se quiser, posso detectar automaticamente dps

    fileLog->Log("[Variants] Base name: " + baseName);
    fileLog->Log("[Variants] Extension: " + ext);

    // 🔹 Carrega o arquivo original primeiro
    fileLog->Log("[Variants] ✔ Loaded original");
    sounds.push_back(fullPath);

    // 🔹 Começa procurar as variantes seguindo padrão *_2.wav, *_3.wav, ...
    for (int i = 2; ; i++)
    {
        std::string candidate = baseName + std::to_string(i) + ext;

        fileLog->Log("[Variants] Checking: " + candidate);

        if (!fs::exists(candidate))
        {
            fileLog->Error("[Variants] ❌ Not found. Stopping.");
            break;
        }

        fileLog->Log("[Variants] ✔ Found variant");
        sounds.push_back(candidate);
    }

    fileLog->Log("[Variants] Total loaded = " + std::to_string(sounds.size()));
    return sounds;
}

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
    fileLog->Log("[AudioVariationGroup] Registered audio: " + src);
}

void AudioVariationGroup::FindAndLoadAudioVariants(std::string src, bool in3d)
{
    auto variants = GetSoundVariants(src);

    for(auto variant : variants)
    {
        LoadNewAudio(variant, in3d);
    }
}