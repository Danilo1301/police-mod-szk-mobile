#include "CNHWindow.h"
#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "DocsWindow.h"

CNHWindow::CNHWindow(Ped* ped) : DocumentWindow("CNH", ped)
{
    
}

CNHWindow::~CNHWindow()
{

}

void CNHWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem("item", 600);

        auto rgContainer = menuSZK->CreateContainer("cnhContainer");
        rgContainer->fillHorizontal = true;
        rgContainer->fillVertical = true;
        rgContainer->SetBackgroundImage(modData->GetFileFromAssets("cnh.png"));

        item->container->AddChild(rgContainer);
        
        auto createLabel = [rgContainer]() -> IContainer* {

            auto label = menuSZK->CreateContainer("label");
            label->text = "This is a text Label";
            label->textHorizontalAlign = HorizontalAlign::Left;
            label->textVerticalAlign = VerticalAlign::Middle;
            label->size = CVector2D(200, 50);
            label->drawBackground = false;

            auto font = &label->textFont;
            font->color = CRGBA(230, 230, 230);
            font->fontSize = 1.6f;

            rgContainer->AddChild(label);

            return label;
        };

        {
            //Nome

            auto label = createLabel();
            label->text = ped->name;
            label->localOffset = CVector2D(155, 138);
        }

        {
            //CPF

            auto label = createLabel();
            label->text = ped->cpf;
            label->localOffset = CVector2D(460, 265);
        }

        {
            //Data Nascimento

            auto label = createLabel();
            label->text = ped->birthDate;
            label->localOffset = CVector2D(460, 335);
        }

        {
            //Cat Hab

            auto label = createLabel();
            label->text = ped->catHab;
            label->textFont.color = CRGBA(255, 0, 0);
            label->localOffset = CVector2D(650, 490);
        }

        {
            //Validade

            auto label = createLabel();
            label->text = ped->cnhExpireDate;
            label->localOffset = CVector2D(455, 550);
        }
    }
    
    {
        auto self = this;

        auto button = window->AddButton("~r~" + GetTranslatedText("close"));
        button->onClick->Add([self, this]() {
            window->Close();

            auto fn = onClose;

            delete self;

            if(fn)
                fn();
        });
    }
}