#include "BottomMessage.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

std::vector<BasicBMessage*> BottomMessage::messages;

ILabel* bottomMessageLabel = NULL;

void BottomMessage::Initialize()
{
    auto label = bottomMessageLabel = menuSZK->CreateLabel();
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

void BottomMessage::AddMessage(std::string title, std::string message, int time)
{
    auto dialog = new BasicBMessage();
    dialog->title = title;
    dialog->message = message;
    dialog->time = time;

    messages.push_back(dialog);
}

void BottomMessage::SetMessage(std::string title, std::string message, int time)
{
    for(auto message : messages)
    {
        message->time = 0;
    }
    AddMessage(title, message, time);
}

void BottomMessage::SetMessage(std::string message, int time)
{
    SetMessage("", message, time);
}

void BottomMessage::AddOfficerDialog(std::string message, int time)
{
    AddMessage("~b~[Policial]", message, time);
}

void BottomMessage::Update()
{
    int deltaTime = menuSZK->deltaTime;

    if(messages.size() > 0)
    {
        auto dialog = messages[0];

        dialog->time -= deltaTime;
        if(dialog->time <= 0)
        {
            // remove
            messages.erase(messages.begin());
            delete dialog;
        }
    }

    if(bottomMessageLabel)
    {
        bottomMessageLabel->GetContainer()->isVisible = HasMessages();

        if(HasMessages())
        {
            auto dialog = messages[0];

            if(dialog->title.empty())
            {
                bottomMessageLabel->text = dialog->message;
            } else {
                bottomMessageLabel->text = dialog->title + "~w~" + dialog->message;
            }
        }
    }
}