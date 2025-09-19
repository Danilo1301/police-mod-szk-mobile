
#pragma once

#include "DocumentWindow.h"

class RGWindow : public DocumentWindow {
public:
    RGWindow(Ped* ped);

    void MakeWindow() override;
    
    static RGWindow* CreateRG(Ped* ped) {
        auto window = new RGWindow(ped);
        window->MakeWindow();
        return window;
    }
};