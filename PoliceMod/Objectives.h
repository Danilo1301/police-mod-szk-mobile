#pragma once

#include "pch.h"

class Objectives {
private:
    static std::string currentObjective;
public:
    static void Initialize();
    static void SetObjective(std::string objective);
    static void ClearObjective();
    static void Update();
};