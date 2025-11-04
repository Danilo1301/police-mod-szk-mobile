#include "FriskWindow.h"

void FriskWindow::OpenForPed(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_frisk"));
    
    {
        auto itemCount = ped->inventory.GetItems().size();

        if(itemCount == 0)
        {
            window->AddText("~r~Nothing here...");
        }
    }

    for(const auto& item : ped->inventory.GetItems())
    {
        std::string text = item.GetDefinition()->name;
        text += " x" + std::to_string(item.GetAmount());

        auto button = window->AddButton(text);
        button->onClick->Add([]() {
        });
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}
