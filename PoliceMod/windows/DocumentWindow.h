#pragma once

#include "pch.h"

#include "menuSZK/IWindow.h"
#include "menuSZK/IContainer.h"
#include "Ped.h"

class DocumentWindow {
protected:
    IWindow* window;
    IContainer* container;
public:
    DocumentWindow(std::string title);

    virtual void MakeWindow(Ped* ped);
};