#pragma once

#include "pch.h"

#include "menuSZK/IAudio.h"

class AudioVariationGroup {
public:
    std::vector<IAudio*> audios;

    IAudio* GetRandomAudio();
    IAudio* PlayRandom();

    void LoadNewAudio(std::string src, bool in3d = false);
};