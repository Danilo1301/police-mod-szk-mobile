#pragma once

#include <iostream>
#include <vector>
#include <map>

#include "IDebug.h"
#include "IContainer.h"
#include "IWindow.h"
#include "IWidgetButton.h"
#include "ILabel.h"
#include "IAudio.h"
#include "ISprite.h"
#include "IFont.h"

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

    IEventListener<CVector2D>* onPointerDown;
    IEventListener<CVector2D>* onPointerUp;

    virtual IDebug* GetDebug() = 0;
    virtual IContainer* CreateContainer() = 0;
    virtual ILabel* CreateLabel() = 0;
    virtual IContainer* GetScreenContainer() = 0;
    virtual bool HasRenderedAtLeastOnce() = 0;
    virtual IWindow* CreateWindow(float x, float y, float width, std::string title) = 0;
    virtual void CreateTestWindow() = 0;
    virtual IWidgetButton* CreateWidgetButton(float x, float y, std::string backgroundTexture, std::string foregroundTexture) = 0;
    virtual void* GetCPedFromRef(int ref) = 0;
    virtual void* GetCVehicleFromRef(int ref) = 0;
    virtual CVector2D ConvertWorldPositionToScreenPosition(CVector worldPosition) = 0;
    virtual IAudio* CreateAudio(std::string src) = 0;
    virtual void QueueContainerDestroy(IContainer* container) = 0;
    virtual ISprite* LoadSprite(std::string src) = 0;
    virtual void DrawSprite(ISprite* sprite, CVector2D position, CVector2D size, CRGBA color) = 0;
    virtual void DrawString(const std::string text, CVector2D position, CRGBA color, IFont font) = 0;
};