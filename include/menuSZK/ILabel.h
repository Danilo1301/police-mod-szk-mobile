#pragma once

#include "IContainer.h"

class ILabel {
public:
    std::string text = "Label";
    CRGBA textColor = CRGBA(255, 255, 255);
    float fontSize = 1.0f;

    HorizontalAlign textHorizontalAlign = HorizontalAlign::Middle;
    VerticalAlign textVerticalAlign = VerticalAlign::Middle;

    float textMarginLeft = 0;

    virtual IContainer* GetContainer() = 0;
};

