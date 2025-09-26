
#pragma once

#include "Vehicle.h"
#include "menuSZK/IMenuSZK.h"

class CellphoneWindow {
private:
    IContainer* container;

protected:
    CellphoneWindow();

private:
    void MakeSINESP(Vehicle* vehicle);

public:
    static CellphoneWindow* CreateSINESPWindow(Vehicle* vehicle) {
        auto window = new CellphoneWindow();
        window->MakeSINESP(vehicle);
        return window;
    }
};