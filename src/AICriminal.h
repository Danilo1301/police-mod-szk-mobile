#pragma once

#include "pch.h"
#include "AIPed.h"

class AICriminal : public AIPed {
protected:

public:
    ~AICriminal() override;

    void Start() override;
    void Update() override;
};