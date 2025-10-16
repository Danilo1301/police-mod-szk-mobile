#include "globals.h"

#include "Peds.h"

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

                wWidget.widget->SetPosition(screenPos.x, screenPos.y);
            }
        }
    }
}