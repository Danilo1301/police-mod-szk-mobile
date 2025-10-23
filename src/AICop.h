#pragma once

#include "pch.h"
#include "AIPed.h"

class AICop : public AIPed {
protected:

public:
    ~AICop() override;

    void Start() override;
    void Update() override;
};