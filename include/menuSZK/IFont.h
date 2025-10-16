#pragma once

#include "simpleGta.h"

struct IFont {
public:
    float fontSize = 1.0f;
    eFontAlignment align = eFontAlignment::ALIGN_LEFT;
    eFontStyle style = eFontStyle::FONT_SUBTITLES;
    CRGBA color = CRGBA(255, 255, 255);
};