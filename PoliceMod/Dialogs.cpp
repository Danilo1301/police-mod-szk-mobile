#include "Dialogs.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

std::vector<BasicDialog*> Dialogs::dialogs;

ILabel* dialogsLabel = NULL;

void Dialogs::Initialize()
{
    auto label = dialogsLabel = menuSZK->CreateLabel();
    label->textHorizontalAlign = HorizontalAlign::Middle;
    label->textVerticalAlign = VerticalAlign::Middle;
    label->fontSize = 0.8f;

    auto container = label->GetContainer();
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Bottom;
    container->localOffset = CVector2D(0, -100);
    container->drawBackground = false; //ativar pra testar

    menuSZK->GetScreenContainer()->AddChild(label->GetContainer());
}

void Dialogs::AddDialog(std::string message, int time)
{
    auto dialog = new BasicDialog();
    dialog->title = "";
    dialog->message = message;
    dialog->time = time;

    dialogs.push_back(dialog);
}

void Dialogs::AddDialog(std::string title, std::string message, int time)
{
    auto dialog = new BasicDialog();
    dialog->title = title;
    dialog->message = message;
    dialog->time = time;

    dialogs.push_back(dialog);
}

void Dialogs::Update()
{
    int deltaTime = menuSZK->deltaTime;

    if(dialogs.size() > 0)
    {
        auto dialog = dialogs[0];

        dialog->time -= deltaTime;
        if(dialog->time <= 0)
        {
            // remove
            dialogs.erase(dialogs.begin());
            delete dialog;
        }
    }

    if(dialogsLabel)
    {
        dialogsLabel->GetContainer()->isVisible = HasDialogs();

        if(HasDialogs())
        {
            auto dialog = dialogs[0];

            if(dialog->title.empty())
            {
                dialogsLabel->text = dialog->message;
            } else {
                dialogsLabel->text = dialog->title + "~w~" + dialog->message;
            }
        }
    }
}