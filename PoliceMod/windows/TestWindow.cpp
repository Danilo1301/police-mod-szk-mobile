#include "TestWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "PoliceMod.h"
#include "Audios.h"
#include "ScriptTask.h"
#include "ModelLoader.h"
#include "CleoFunctions.h"
#include "BottomMessage.h"

static void CreateTowTruck()
{
    int towModelId = 578;
    int testCar = 560;
    int driverId = 50;

    ModelLoader::AddModelToLoad(towModelId);
    ModelLoader::AddModelToLoad(driverId);
    ModelLoader::AddModelToLoad(testCar);

    ModelLoader::LoadAll([towModelId, driverId, testCar]() {
        
        auto playerPosition = GetPlayerPosition();

        auto spawnPosition = GetPlayerPositionInForward(5.0f);

        BottomMessage::SetMessage("Criando veiculos", 3000);

        auto towRef = CREATE_CAR_AT(towModelId, spawnPosition.x, spawnPosition.y, spawnPosition.z);
        auto testCarRef = CREATE_CAR_AT(testCar, spawnPosition.x, spawnPosition.y, spawnPosition.z + 3);

        CVector offset = CVector(0, -0.5, 0.3);

        ATTACH_CAR_TO_CAR(testCarRef, towRef, offset.x, offset.y, offset.z, 0, 0, 0);
    });
}

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
        auto button = window->AddButton("> Test task");
        button->onClick->Add([window](IContainer*) {
            window->Close();
            
            ScriptTask::CreateTest();
        });
    }

    {
        auto button = window->AddButton("> Test tow truck");
        button->onClick->Add([window](IContainer*) {
            window->Close();
            
            CreateTowTruck();
        });
    }

    {
        auto button = window->AddButton("> ~r~Fechar");
        button->onClick->Add([window](IContainer*) {
            window->Close();
        });
    }
}