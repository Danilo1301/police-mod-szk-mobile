
#pragma once

#include "DocumentWindow.h"

class RGWindow : public DocumentWindow {
public:
    RGWindow();

    void MakeWindow(Ped* ped) override;
    
    static RGWindow* CreateRG(Ped* ped) {
        auto window = new RGWindow();
        window->MakeWindow(ped);
        return window;
    }
};