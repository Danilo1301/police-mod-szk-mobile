#include "TestWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "PoliceMod.h"
#include "Audios.h"

TestWindow::TestWindow()
{
    auto window = menuSZK->CreateWindow(400, 50, 900, "Testes - Mod Policia");

    {
        auto button = window->AddButton("Equipar");
        button->onClick->Add([](IContainer*) {
            policeMod->TestEquip();
        });
    }

    {
        auto button = window->AddButton("Audio - Consultar placa");
        button->onClick->Add([](IContainer*) {

            auto audioGroup = new AudioGroup();
            audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets("audios/ht.wav")));
            audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets("audios/officer/license_plate_check.wav")));

            auto plate = randomPlateLimited();

            debug->AddLine("Consulta de placa: ~y~" + plate);

            // percorrendo cada caractere
            for (size_t i = 0; i < plate.size(); ++i)
            {
                char c = plate[i];

                if (c == ' ') continue; // pula espaços

                // monta o caminho do áudio dinamicamente
                std::string audioPath = "audios/officer/codes/";
                audioPath += c;        // adiciona a letra ou número
                audioPath += ".wav";   // extensão

                audioGroup->AddAudio(menuSZK->CreateAudio(getPathFromAssets(audioPath)));
            }

            audioGroup->Play();
        });
    }

    {
        auto button = window->AddButton("> ~r~Fechar");
        button->onClick->Add([window](IContainer*) {
            window->Close();
        });
    }
}