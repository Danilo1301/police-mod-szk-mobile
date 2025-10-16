#pragma once

#include "IContainer.h"

class IWindowItem {
public:
    IContainer* container;
    float height = 90;
};