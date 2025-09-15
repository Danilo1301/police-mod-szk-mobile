#pragma once

#include "IContainer.h"
#include "IWindowItem.h"
#include "IWindowItemButton.h"

#include <vector>

class IWindow {
public:
    IContainer* container = NULL;

    IContainer* subtitleContainer = NULL;

    std::vector<IWindowItem*> items;

    int maxItemsPerPage = 6;

    virtual IWindowItem* AddText(std::string text) = 0;
    virtual IWindowItemButton* AddButton(std::string text) = 0;
    virtual IWindowItem* AddCustomItem() = 0;
    virtual IWindowItem* AddOptions(std::string text) = 0;
    virtual IWindowItem* AddIntOptions(std::string text, int* ptr, int min, int max) = 0;
    virtual IWindowItem* AddFloatOptions(std::string text, float* ptr, float min, float max) = 0;
    virtual IWindowItem* AddCheckbox(std::string text, bool* ptr) = 0;

    virtual void Close() = 0;
};