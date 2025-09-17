#pragma once

#include "pch.h"

#include "menuSZK/IAudio.h"

class Audios {
public:
    static IAudio* audioEncostaCarro;
    static IAudio* audioDesceMaoCabeca;
    static IAudio* audioLiberado;

    static void CreateAudios();
};