#include "RGWindow.h"
#include "BottomMessage.h"
#include "CleoFunctions.h"
#include "DocsWindow.h"

RGWindow::RGWindow(Ped* ped) : DocumentWindow("RG", ped)
{
    
}

RGWindow::~RGWindow()
{

}

void RGWindow::MakeWindow()
{
    DocumentWindow::MakeWindow();

    {
        auto item = window->AddCustomItem("item", 600);

        auto rgContainer = menuSZK->CreateContainer("rgContainer");
        rgContainer->fillHorizontal = true;
        rgContainer->fillVertical = true;
        rgContainer->SetBackgroundImage(modData->GetFileFromAssets("rg.png"));

        item->container->AddChild(rgContainer);
        
        auto createRGLabel = [rgContainer]() -> IContainer* {

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
            //CPF

            auto label = createRGLabel();
            label->text = ped->cpf;
            label->localOffset = CVector2D(140, 65);
        }

        {
            //RG

            auto label = createRGLabel();
            label->text = ped->rg;
            label->localOffset = CVector2D(350, 120);
        }

        {
            //Nome

            auto label = createRGLabel();
            label->text = ped->name;
            label->localOffset = CVector2D(180, 178);
        }

        {
            //Data Nascimento

            auto label = createRGLabel();
            label->text = ped->birthDate;
            label->localOffset = CVector2D(310, 310);
        }

        {
            //Ped ID

            auto label = createRGLabel();
            label->textFont.align = eFontAlignment::ALIGN_RIGHT;
            label->text = std::to_string(ped->ref);
            label->textHorizontalAlign = HorizontalAlign::Right;
            label->localOffset = CVector2D(590, 90);
        }
    }

    {
        auto button = window->AddButton("> Consultar");
        button->onClick->Add([this]() {
            window->Close();

            BottomMessage::SetMessage(GetTranslatedText("checking_rg"), 5000);

            WAIT(5000, [this]() {
                DocsWindow::ShowRGResults(ped);
            });
        });
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