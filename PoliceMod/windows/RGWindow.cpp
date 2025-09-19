#include "RGWindow.h"

#include <utils.h>
#include <menuSZK/ILabel.h>

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

RGWindow::RGWindow(Ped* ped) : DocumentWindow("RG", ped)
{
    
}

void RGWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem();
        item->height = 600;

        auto container = item->container;

        auto rgContainer = container->CreateChildContainer();
        rgContainer->fillHorizontal = true;
        rgContainer->fillVertical = true;
        rgContainer->SetBackgroundTexture(getPathFromAssets("rg.png"));

        auto createRGLabel = [rgContainer]() -> ILabel* {

            auto label = menuSZK->CreateLabel();
            label->textColor = CRGBA(180, 180, 180);
            label->text = "This is a text Label";
            label->textHorizontalAlign = HorizontalAlign::Left;
            label->textVerticalAlign = VerticalAlign::Middle;
            label->fontSize = 0.6f;

            auto labelContainer = label->GetContainer();
            labelContainer->size = CVector2D(200, 50);
            labelContainer->drawBackground = false;

            rgContainer->AddChild(labelContainer);

            return label;
        };

        {
            //CPF

            auto label = createRGLabel();
            label->text = ped->cpf;
            label->GetContainer()->localOffset = CVector2D(140, 65);
        }

        {
            //RG

            auto label = createRGLabel();
            label->text = ped->rg;
            label->GetContainer()->localOffset = CVector2D(350, 120);
        }

        {
            //Nome

            auto label = createRGLabel();
            label->text = ped->name;
            label->GetContainer()->localOffset = CVector2D(180, 178);
        }

        {
            //Data Nascimento

            auto label = createRGLabel();
            label->text = ped->birthDate;
            label->GetContainer()->localOffset = CVector2D(310, 310);
        }

        {
            //Ped ID

            auto label = createRGLabel();
            label->text = std::to_string(ped->ref);
            label->textHorizontalAlign = HorizontalAlign::Right;
            label->GetContainer()->localOffset = CVector2D(590, 90);
        }
    }

    {
        auto button = window->AddButton("> Opcoes");
        button->onClick->Add([this](IContainer*) {
            //window->Close();
        });
    }

    {
        auto button = window->AddButton("> ~r~Fechar");
        button->onClick->Add([this](IContainer*) {
            window->Close();

            if(onClose)
                onClose();
        });
    }
}