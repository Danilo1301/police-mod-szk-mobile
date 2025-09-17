#pragma once

#include "pch.h"

class PoliceMod {
public:
    PoliceMod();

    void Initialize();
    void Update();

    void TestEquip();
};

extern PoliceMod* policeMod;