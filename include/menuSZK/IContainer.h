#pragma once

#include "simpleGta.h"

#include "IFont.h"
#include "IEventListener.h"

#include <string>

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

enum class ClickThroughMode
{
    CantClickThrough,
    ClickThroughThisOnly,
    ClickThroughThisAndChildren
};

class IContainer {
public:
    IContainer* parent = NULL;
    std::vector<IContainer*> children;
    std::string tag = "container";

    CVector2D localOffset = CVector2D(0, 0);
    CVector2D size = CVector2D(200, 200);
    CRGBA backgroundColor = CRGBA(255, 255, 255, 50);
    bool drawBackground = true;

    HorizontalAlign horizontalAlign = HorizontalAlign::Left;
    bool fillHorizontal = false;
    float fillHorizontalPercentage = 1.0f;

    VerticalAlign verticalAlign = VerticalAlign::Top;
    bool fillVertical = false;

    bool isVisible = true;
    ClickThroughMode clickThroughMode = ClickThroughMode::CantClickThrough;
    bool canBeDragged = false;
    bool hideWhenPauseGame = false;

    bool isAnyPointerOver = false;

    CRGBA highlightColor = CRGBA(255, 255, 255, 0);

    std::string text = "";
    IFont textFont;

    HorizontalAlign textHorizontalAlign = HorizontalAlign::Left;
    VerticalAlign textVerticalAlign = VerticalAlign::Top;
    float textMarginLeft = 0;

    IEventListener<>* onClick;
    IEventListener<>* onBeforeDraw;
    IEventListener<>* onAfterDraw;

    virtual ~IContainer() = default;

    virtual IContainer* AddChild(IContainer* child) = 0;
    virtual void RemoveChild(IContainer* child, bool destroy = false) = 0;

    virtual void SetBackgroundImage(std::string src) = 0;
    virtual void SetBackgroundImage(void* texture) = 0;
};