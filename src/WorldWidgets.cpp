#include "WorldWidgets.h"

#include "Peds.h"
#include "Vehicles.h"

void UpdateWorldWidgets()
{
    for(auto wWidget : WorldWidgets)
    {
        if(wWidget.attachToPed != -1)
        {
            auto ped = Peds::GetPed(wWidget.attachToPed);

            if(Peds::IsValid(ped))
            {
                auto screenPos = menuSZK->ConvertWorldPositionToScreenPosition(ped->GetPosition());
                auto size = wWidget.widget->GetSize();

                wWidget.widget->SetPosition(screenPos.x - size.x/2, screenPos.y - size.y/2);
            }
        }

        if(wWidget.attachToVehicle != -1)
        {
            auto vehicle = Vehicles::GetVehicle(wWidget.attachToVehicle);

            if(Vehicles::IsValid(vehicle))
            {
                auto screenPos = menuSZK->ConvertWorldPositionToScreenPosition(vehicle->GetPosition());
                auto size = wWidget.widget->GetSize();

                wWidget.widget->SetPosition(screenPos.x - size.x/2, screenPos.y - size.y/2);
            }
        }
    }
}