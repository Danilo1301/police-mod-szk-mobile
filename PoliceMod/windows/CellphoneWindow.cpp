#include "CellphoneWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

ILabel* createLabel() {
    auto label = menuSZK->CreateLabel();
    label->textColor = CRGBA(180, 180, 180);
    label->text = "This is a text Label";
    label->textHorizontalAlign = HorizontalAlign::Left;
    label->textVerticalAlign = VerticalAlign::Middle;
    label->fontSize = 0.6f;

    auto labelContainer = label->GetContainer();
    labelContainer->size = CVector2D(300, 50);
    labelContainer->drawBackground = false;

    return label;
};

CellphoneWindow::CellphoneWindow()
{
    container = menuSZK->CreateContainer();
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Middle;
    container->size = CVector2D(540, 1100);
    container->drawBackground = false;
    menuSZK->GetScreenContainer()->AddChild(container);
}

void CellphoneWindow::MakeSINESP(Vehicle* vehicle)
{
    auto backgroundImage = getPathFromAssets("cellphone/sinesp_ok.png");
    auto closeImage = getPathFromAssets("cellphone/close.png");

    container->SetBackgroundTexture(backgroundImage);

    {
        //Placa

        auto label = createLabel();
        label->text = vehicle->plate;
        label->GetContainer()->localOffset = CVector2D(0, 400);
        label->GetContainer()->horizontalAlign = HorizontalAlign::Middle;
        label->textHorizontalAlign = HorizontalAlign::Middle;
        label->textVerticalAlign = VerticalAlign::Middle;
        container->AddChild(label->GetContainer());
    }

    {
        //Ano do modelo

        auto label = createLabel();
        label->text = std::to_string(vehicle->modelYear);
        label->GetContainer()->localOffset = CVector2D(180, 485);
        label->textHorizontalAlign = HorizontalAlign::Right;
        container->AddChild(label->GetContainer());
    }

    {
        //Ano de fabricação

        auto label = createLabel();
        label->text = std::to_string(vehicle->manufactureYear);
        label->GetContainer()->localOffset = CVector2D(180, 535);
        label->textHorizontalAlign = HorizontalAlign::Right;
        container->AddChild(label->GetContainer());
    }

    {
        //Final do chassis

        auto chassis = vehicle->chassis; // ex: "9BWZZZ377VT004251"
        auto finalNumbers = chassis.substr(chassis.size() - 6); // pega os 6 últimos caracteres

        auto label = createLabel();
        label->text = "...." + finalNumbers;
        label->GetContainer()->localOffset = CVector2D(180, 585);
        label->textHorizontalAlign = HorizontalAlign::Right;
        container->AddChild(label->GetContainer());
    }

    {
        //Documentação

        auto label = createLabel();
        label->text = vehicle->outdatedDoc ? "~r~Atrasada" : "Em dia";
        label->GetContainer()->localOffset = CVector2D(180, 633);
        label->textHorizontalAlign = HorizontalAlign::Right;
        container->AddChild(label->GetContainer());
    }

    {
        auto button = menuSZK->CreateContainer();
        button->size = CVector2D(400, 60);
        button->horizontalAlign = HorizontalAlign::Middle;
        button->verticalAlign = VerticalAlign::Middle;
        button->localOffset = CVector2D(0, 250);
        button->SetBackgroundTexture(closeImage);
        container->AddChild(button);

        button->onClick->Add([this](IContainer*) {
            container->SetToBeDestroyed();
        });
    }
}