#pragma once

#include "pch.h"

#include "Checkpoint.h"

class PoliceBase {
public:
    CVector basePosition = CVector(0, 0, 0);

    CVector leaveCriminalPosition = CVector(0, 0, 0);
    Checkpoint* leaveCriminalCheckpoint = nullptr;

    CVector getPartnerPosition = CVector(0, 0, 0);
    Checkpoint* getPartnerCheckpoint = nullptr;

    PoliceBase();
    
    void Update();
    void OnPostDrawRadar();
    void OnDraw();
};