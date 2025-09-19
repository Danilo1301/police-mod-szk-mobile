#include "CRLVWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

CRLV_Window::CRLV_Window(Ped* ped, Vehicle* vehicle) : DocumentWindow("CRLV", ped)
{
    this->vehicle = vehicle;
}

void CRLV_Window::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem();
        item->height = 600;

        auto container = item->container;

        auto rgContainer = container->CreateChildContainer();
        rgContainer->fillHorizontal = true;
        rgContainer->fillVertical = true;
        rgContainer->SetBackgroundTexture(getPathFromAssets("crlv.png"));

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
            //Codigo REN

            auto label = createRGLabel();
            label->text = vehicle->renavamCode;
            label->GetContainer()->localOffset = CVector2D(42, 175);
        }

        {
            //Placa

            auto label = createRGLabel();
            label->text = vehicle->plate;
            label->GetContainer()->localOffset = CVector2D(42, 245);
        }

        {
            //Exercício

            auto label = createRGLabel();
            label->text = std::to_string(vehicle->exerciseYear);
            label->GetContainer()->localOffset = CVector2D(232, 245);
        }

        {
            //Ano Fabricação

            auto label = createRGLabel();
            label->text = std::to_string(vehicle->manufactureYear);
            label->GetContainer()->localOffset = CVector2D(415, 245);
        }

        {
            //Ano Modelo

            auto label = createRGLabel();
            label->text = std::to_string(vehicle->modelYear);
            label->GetContainer()->localOffset = CVector2D(42, 314);
        }

        {
            //Chassis

            auto label = createRGLabel();
            label->text = vehicle->chassis;
            label->GetContainer()->localOffset = CVector2D(42, 385);
        }

        {
            //Nome

            auto label = createRGLabel();
            label->text = ped->name;
            label->GetContainer()->localOffset = CVector2D(42, 453);
        }

        {
            //CPF / CNPJ

            auto label = createRGLabel();
            label->text = ped->cpf;
            label->GetContainer()->localOffset = CVector2D(42, 520);
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