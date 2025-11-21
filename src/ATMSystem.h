#pragma once

#include "pch.h"

struct Location
{
    CVector position;
    CVector rotation;
};

class ATMSystem {
public:
    static void Initialize();
    static void CreateObjects();

    static Location* GetRandomATMLocation();
    static Location* GetATMLocation(CVector position);
};