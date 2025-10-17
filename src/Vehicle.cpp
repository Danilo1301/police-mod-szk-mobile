#include "Vehicle.h"

#include "Pullover.h"
#include "CleoFunctions.h"
#include "WorldWidgets.h"

Vehicle::Vehicle(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    {
        auto widget = menuSZK->CreateWidget(
            200,
            200,
            100,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );
        widget->visible = false;

        WorldWidget w;
        w.widget = widget;
        w.attachToVehicle = ref;
        WorldWidgets.push_back(w);

        widget->onClick->Add([]() {
            //Pullover::OpenPedMenu(this);
        });

        widgetOptions = widget;
    }
}

Vehicle::~Vehicle()
{
    if(widgetOptions)
    {
        widgetOptions->Close();
        widgetOptions = nullptr;
    }
}

void Vehicle::Update()
{
    
}

void Vehicle::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Vehicle::HideBlip()
{
    flags.showBlip = false;
}

CVector Vehicle::GetPosition()
{
    return GetCarPosition(ref);
}

bool Vehicle::IsPlayerInside()
{
    return false;
}