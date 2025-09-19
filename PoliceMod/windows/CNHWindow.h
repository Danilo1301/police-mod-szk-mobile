
#pragma once

#include "DocumentWindow.h"

class CNHWindow : public DocumentWindow {
public:
    CNHWindow();

    void MakeWindow(Ped* ped) override;
    
    static CNHWindow* CreateCNH(Ped* ped) {
        auto window = new CNHWindow();
        window->MakeWindow(ped);
        return window;
    }
};