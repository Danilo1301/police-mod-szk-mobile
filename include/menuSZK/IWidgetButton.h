#pragma once

#include "simpleGta.h"
#include "IEventListener.h"
#include "IContainer.h"

class IWidgetButton {
public:
    IEventListener<>* onClickWidget;

    CRGBA pointerOverColor = CRGBA(90, 180, 100);

    virtual void Close() = 0;
    virtual IContainer* GetContainer() = 0;
};