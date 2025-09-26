#pragma once

#include "pch.h"

#include "menuSZK/IMenuSZK.h"

struct RadioButton {
public:
    std::string id;
    std::string image;
    IContainer* container = nullptr;

    RadioButton(std::string id, std::string image)
        : id(id), image(image) {}

    void Show(float x, float y)
    {
        container->localOffset = CVector2D(x, y);
        container->isVisible = true;
    }

    void Hide()
    {
        container->isVisible = false;
    }
};

struct RadioPage {
public:
    std::string id;                  
    std::vector<RadioButton*> buttons;

    RadioPage(std::string id)
        : id(id) {}
};

class RadioWindow {
public:
    static std::unordered_map<std::string, RadioPage*> pages;

    static void Initialize();
    static void Toggle();
private:
    static void ShowButtons();
    static void HideButtons();
    static std::vector<RadioButton*> GetAllButtons();
    static void GotoPageIndex(int pageIndex);
    static void GotoPageId(std::string pageId);

    static void OnClickButton(std::string id);
};