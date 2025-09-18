#pragma once

#include "pch.h"

class PoliceMod {
public:
    static bool m_IsUsingMenu;

    PoliceMod();

    void Initialize();
    void Update();

    void TestEquip();
};

extern PoliceMod* policeMod;