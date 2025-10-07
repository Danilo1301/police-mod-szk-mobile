#pragma once

#include "pch.h"

#define NO_CHECKPOINT -1

class Checkpoint {
private:
    int checkpoint = NO_CHECKPOINT;
    CVector lastPosition;
public:
    CVector position;
    float radius = 2.0f;

    void Update();
    bool IsInRange(CVector point);
    void DestroyCheckpoint();
};

class Checkpoints {
public:
    static std::vector<Checkpoint*> AllCheckpoints;

    static void Update();
    static Checkpoint* CreateCheckpoint(CVector position);
    static void DestroyCheckpoint(Checkpoint* checkpoint);
};