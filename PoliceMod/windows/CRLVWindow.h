
#pragma once

#include "DocumentWindow.h"

#include "Vehicle.h"

class CRLV_Window : public DocumentWindow {
public:
    Vehicle* vehicle;

    CRLV_Window(Ped* ped, Vehicle* vehicle);

    void MakeWindow() override;
    
    static CRLV_Window* Create(Ped* ped, Vehicle* vehicle) {
        auto window = new CRLV_Window(ped, vehicle);
        window->MakeWindow();
        return window;
    }
};