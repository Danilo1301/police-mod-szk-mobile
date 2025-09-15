#pragma once

#include "IEventListener.h"

class IWindowItemButton {
public:
    IEventListener<IContainer*>* onClick;
};