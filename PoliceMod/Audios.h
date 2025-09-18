#pragma once

#include "pch.h"

#include "menuSZK/IAudio.h"

class Audios {
public:
    static IAudio* audioEncostaCarro;
    static IAudio* audioDesceMaoCabeca;
    static IAudio* audioLiberado;
    static IAudio* audioPedirRG;

    static void CreateAudios();
    static void Update();
};

class AudioGroup {
private:
    std::vector<IAudio*> audios;
    int currentAudio = 0;
    bool hasCompleted = false;

    void PlayNext();
public:

    AudioGroup();

    void AddAudio(IAudio* audio);
    void Play();
    bool HasCompleted();
    void RemoveAndDestroyAudios();
};

static AudioGroup* currentAudioGroup = NULL;