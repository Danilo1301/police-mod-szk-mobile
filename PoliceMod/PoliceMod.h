#pragma once

#include "pch.h"

class PoliceMod {
public:
    PoliceMod();

    void Initialize();
    void Update();
};

extern PoliceMod* policeMod;