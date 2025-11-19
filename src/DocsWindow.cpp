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

void DocsWindow::ShowVehicleVisualInfo(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle"));

    if(vehicle == nullptr) return;

    {
        window->AddText("- Placa: " + vehicle->currentDoc.plate);
    }

    {
        window->AddText("- Cor: ?");
    }

    {
        auto button = window->AddButton(GetTranslatedText("check_vehicle_by_plate"));
        button->onClick->Add([window, vehicle]() {
            window->Close();

            BottomMessage::SetMessage(GetTranslatedText("checking_plate"), 5000);

            auto audio = audioPlateCheck->GetRandomAudio();

            RadioSounds::PlayAudioNowDontAttach(audio);

            WaitForAudioFinish(audio, [vehicle]() {
                ShowVehicleResults(vehicle, true);
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

void DocsWindow::ShowVehicleResults(Vehicle* vehicle, bool byPlate)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle_results"));
    
    auto doc = byPlate ? vehicle->currentDoc : vehicle->originalDoc;

    window->AddText("- Placa: " + doc.plate);
    window->AddText("- Chassi: " + doc.chassis);

    if(doc.isStolen)
    {
        window->AddText("- " + GetTranslatedText("vehicle_stolen"));
    } else {
        window->AddText("- Nao tem queixa de roubo");
    }

    if(doc.isDocumentExpired)
    {
        window->AddText("- " + GetTranslatedText("vehicle_doc_expired"));
    } else {
        window->AddText("- " + GetTranslatedText("vehicle_doc_ok"));
    }

    if(vehicle->flags.swappedPlate)
    {
        window->AddText("- ~r~As caracteristicas nao batem");

        if(byPlate)
        {
            auto button = window->AddButton(GetTranslatedText("check_vehicle_by_chassis"));
            button->onClick->Add([window, vehicle]() {
                window->Close();

                BottomMessage::SetMessage(GetTranslatedText("checking_chassis"), 5000);

                WAIT(5000, [vehicle]() {
                    ShowVehicleResults(vehicle, false);
                });
            });
        }

    } else {
        window->AddText("- As caracteristicas do veiculo batem");
    }
    
    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}

void DocsWindow::ShowCRLV(Ped* ped, Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle_results"));
    
    window->AddText("Placa: " + vehicle->originalDoc.plate);
    window->AddText("RENAVAM: " + vehicle->originalDoc.renavam);
    window->AddText("Chassi: " + vehicle->originalDoc.chassis);
    window->AddText("CPF: " + ped->cpf);

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

        // if(ped->flags.expiredDriversLicense)
        // {
        //     window->AddText(GetTranslatedText("vehicle_cnh_expired"));
        // } else {
        //     window->AddText(GetTranslatedText("vehicle_cnh_ok"));
        // }
        
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

void DocsWindow::ShowChassisResult(Vehicle* vehicle)
{
    auto window = menuSZK->CreateWindow(g_defaultMenuPosition.x, g_defaultMenuPosition.y, 800, GetTranslatedText("window_vehicle_results"));
    
    {
        if(vehicle->flags.chassisErased)
        {
            window->AddText("- Chassis: ~r~Suprimido");
        } else {
            window->AddText("- Chassis: " + vehicle->originalDoc.chassis);
        }
    }

    {
        auto button = window->AddButton("~y~" + GetTranslatedText("close"));
        button->onClick->Add([window]() {
            window->Close();
        });
    }
}