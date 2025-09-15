#pragma once

#include "simpleGta.h"

#include "IEventListener.h"

#include <iostream>
#include <vector>

enum class HorizontalAlign {
    Left = 0,
    Middle,
    Right
};

enum class VerticalAlign {
    Top = 0,
    Middle,
    Bottom
};

class IContainer {
public:
    IContainer* parent = NULL;
    std::vector<IContainer*> children;
    std::string tag = "container";

    bool isVisible = true;
    bool canClickThrough = false;

    CVector2D localOffset = CVector2D(0, 0);
    CVector2D size = CVector2D(200, 200);
    CRGBA backgroundColor = CRGBA(255, 255, 255, 50);
    bool drawBackground = true;

    bool fillHorizontal = false;
    bool fillVertical = false;

    float fillHorizontalPercentage = 1.0f;

    HorizontalAlign horizontalAlign = HorizontalAlign::Left;
    VerticalAlign verticalAlign = VerticalAlign::Top;

    bool isDraggable = false;

    bool isClickable = false;
    bool drawClickableBackground = true;

    float backgroundImageScale = 1.0f;
    CRGBA imageColor = CRGBA(255, 255, 255, 255);

    bool useWorldPosition = false;
    CVector worldPosition = CVector(0, 0, 0);

    bool fixPositionToCenter = false;

    IEventListener<IContainer*>* onClick;
    IEventListener<IContainer*>* onAfterDraw; 

    virtual CVector2D GetPosition() = 0;
    
    virtual IContainer* AddChild(IContainer* child) = 0;
    virtual void RemoveChild(IContainer* child, bool destroy = false) = 0;
    virtual IContainer* CreateChildContainer() = 0;
    virtual void SetBackgroundTexture(std::string src) = 0;
    virtual bool IsContainerOverMouse() = 0;
};