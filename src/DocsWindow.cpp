#include "DocsWindow.h"

#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "Vehicles.h"
#include "AudioCollection.h"
#include "AudioSequence.h"
#include "RadioSounds.h"

void DocsWindow::ShowRG(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_rg"));
    
    {
        window->AddText("- ID: " + std::to_string(ped->ref));
    }

    {
        window->AddText("- RG: " + ped->rg);
        window->AddText("- CPF: " + ped->cpf);
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_rg"));
        button->onClick->Add([window, ped]() {
            window->Close();

            BottomMessage::SetMessage(GetTranslatedText("checking_rg"), 5000);

            WAIT(5000, [ped]() {
                ShowRGResults(ped);
            });
        });
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void DocsWindow::ShowRGResults(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_rg_results"));
    
    if(ped->flags.wantedByJustice)
    {
        window->AddText(GetTranslatedText("suspect_with_warrant"));
    } else {
        window->AddText(GetTranslatedText("suspect_with_no_warrant"));
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void DocsWindow::ShowVehicleDocs(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle"));

    if(vehicle == nullptr) return;

    {
        window->AddText("- ID do veiculo: " + std::to_string(vehicle->ref));
    }

    {
        window->AddText("- Placa: " + vehicle->plate);
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_vehicle_plate"));
        button->onClick->Add([window, vehicle]() {
            window->Close();

            BottomMessage::SetMessage(GetTranslatedText("checking_plate"), 5000);

            auto audio = audioPlateCheck->GetRandomAudio();

            RadioSounds::PlayAudioNowDontAttach(audio);

            WaitForAudioFinish(audio, [vehicle]() {
                ShowVehicleResults(vehicle);
            });
        });
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void DocsWindow::ShowVehicleResults(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle_results"));
    
    if(vehicle->flags.isStolen)
    {
        window->AddText(GetTranslatedText("vehicle_stolen"));
    }

    if(vehicle->flags.hasExpiredDocument)
    {
        window->AddText(GetTranslatedText("vehicle_doc_expired"));
    } else {
        window->AddText(GetTranslatedText("vehicle_doc_ok"));
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void DocsWindow::ShowCNH(Ped* ped)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_cnh"));
    
    if(ped->catHab.length() > 0)
    {
        window->AddText(GetTranslatedText("cnh_category") + ped->catHab);

        if(ped->flags.expiredDriversLicense)
        {
            window->AddText(GetTranslatedText("vehicle_cnh_expired"));
        } else {
            window->AddText(GetTranslatedText("vehicle_cnh_ok"));
        }
        
    } else {
        window->AddText(GetTranslatedText("with_no_cnh"));
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}