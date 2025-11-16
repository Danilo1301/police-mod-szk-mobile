#pragma once

#include "IContainer.h"

class IWindowItem {
public:
    IContainer* container;
    IContainer* titleContainer;
    float height = 90;

    virtual ~IWindowItem() = default;

    virtual void SetTitle(std::string title) = 0;
};