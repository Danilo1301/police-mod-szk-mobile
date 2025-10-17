#pragma once

#include "menuSZK/IWidget.h"

struct WorldWidget
{
    IWidget* widget;
    int attachToPed = -1;
    int attachToVehicle = -1;
};

inline std::vector<WorldWidget> WorldWidgets;

void UpdateWorldWidgets();
