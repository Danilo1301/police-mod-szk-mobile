#pragma once

#include "simpleGta.h"

#include "IWindowItem.h"
#include "IWindowItemButton.h"

#include <string>

class IWindow {
public:
    std::string title = "Titulo";
    std::string subTitle = "";

    virtual ~IWindow() {};

    virtual IWindowItem* AddText(std::string text) = 0;
    virtual IWindowItem* AddCheckbox(std::string text, bool* pBool) = 0;
    virtual IWindowItemButton* AddButton(std::string text) = 0;
    virtual IWindowItem* AddOptions(std::string text) = 0;

    virtual void Close() = 0;
};