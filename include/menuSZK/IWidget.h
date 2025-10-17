#pragma once

#include "simpleGta.h"

#include <string>

#include "IEventListener.h"

class IWidget {
public:
    IEventListener<>* onClick;
    bool visible = true;
    
    virtual ~IWidget() {};

    virtual void SetPosition(float x, float y) = 0;
    virtual CVector2D GetSize() = 0;
    virtual void Close() = 0;
};