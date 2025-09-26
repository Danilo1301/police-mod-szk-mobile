#include "CNHWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

CNHWindow::CNHWindow(Ped* ped) : DocumentWindow("CNH", ped)
{
    
}

void CNHWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem();
        item->height = 600;

        auto container = item->container;

        auto rgContainer = container->CreateChildContainer();
        rgContainer->fillHorizontal = true;
        rgContainer->fillVertical = true;
        rgContainer->SetBackgroundTexture(getPathFromAssets("cnh.png"));

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
            //Nome

            auto label = createRGLabel();
            label->text = ped->name;
            label->GetContainer()->localOffset = CVector2D(155, 138);
        }

        {
            //CPF

            auto label = createRGLabel();
            label->text = ped->cpf;
            label->GetContainer()->localOffset = CVector2D(460, 265);
        }

        {
            //Data Nascimento

            auto label = createRGLabel();
            label->text = ped->birthDate;
            label->GetContainer()->localOffset = CVector2D(460, 335);
        }

        {
            //Cat Hab

            auto label = createRGLabel();
            label->text = ped->catHab;
            label->textColor = CRGBA(255, 0, 0);
            label->GetContainer()->localOffset = CVector2D(650, 490);
        }

        {
            //Validade

            auto label = createRGLabel();
            label->text = ped->cnhExpireDate;
            label->GetContainer()->localOffset = CVector2D(455, 550);
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

            auto fn = onClose;

            delete this;

            if(fn)
                fn();
        });
    }
}