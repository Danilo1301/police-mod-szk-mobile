#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "IDebug.h"
#include "IContainer.h"
#include "IWindow.h"
#include "IWidgetButton.h"

class IMenuSZK {
public:
    unsigned int deltaTime = 0;
    bool debugMode = false;

    IEventListener<>* onUpdate;
    IEventListener<>* onFirstUpdate;

    IEventListener<int>* onPedAdded;
    IEventListener<int>* onPedRemoved;

    IEventListener<int>* onVehicleAdded;
    IEventListener<int>* onVehicleRemoved;

    virtual IDebug* GetDebug() = 0;
    virtual IContainer* CreateContainer() = 0;
    virtual IContainer* GetScreenContainer() = 0;
    virtual bool HasRenderedAtLeastOnce() = 0;
    virtual IWindow* CreateWindow(float x, float y, float width, std::string title) = 0;
    virtual void CreateTestWindow() = 0;
    virtual IWidgetButton* CreateWidgetButton(float x, float y, std::string backgroundTexture, std::string foregroundTexture) = 0;
    virtual void* GetCPedFromRef(int ref) = 0;
    virtual void* GetCVehicleFromRef(int ref) = 0;
    virtual CVector2D ConvertWorldPositionToScreenPosition(CVector worldPosition) = 0;
};