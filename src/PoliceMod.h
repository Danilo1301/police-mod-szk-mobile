#pragma once

#include "pch.h"

class PoliceMod {
public:
    PoliceMod();

    void OnModPreLoad();
    void OnModLoad();
    void OnGameUpdate();
    void OnFirstUpdate();
};

extern PoliceMod* policeMod;