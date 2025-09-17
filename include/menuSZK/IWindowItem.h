#pragma once

#include "IEventListener.h"
#include "IContainer.h"

class IWindowItem {
public:
    IContainer* container;
    float height = 90;
    IEventListener<IContainer*>* onAfterDraw;
};