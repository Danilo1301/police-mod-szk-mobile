#pragma once

#include "pch.h"
#include "AIPed.h"

class AICriminal : public AIPed {
public:
    int targetPed = -1;
    int prevTargetPed = -1;
    int findTargetTimer = 0;

    ~AICriminal() override;

    void Start() override;
    void Update() override;

    void FindTarget();

    void OnTargetChange();
};