#include "BottomMessage.h"

int s_timer = 0.0f;
int s_currentIndex = -1;
IContainer* s_container = nullptr;

std::vector<IMessage> BottomMessage::Messages;

void BottomMessage::Initialize()
{
    auto container = menuSZK->CreateContainer("bottomMessage");
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Bottom;
    container->localOffset = CVector2D(0, -100);
    container->canClickThrough = true;

    container->text = "Aqui vai ficar a mensagme";
    container->textHorizontalAlign = HorizontalAlign::Middle;
    container->textVerticalAlign = VerticalAlign::Middle;

    auto font = &container->textFont;
    font->fontSize = 2.0f;
    font->align = eFontAlignment::ALIGN_CENTER;

    menuSZK->GetRootContainer()->AddChild(container);

    s_container = container;
}

void BottomMessage::Update()
{
    if (!s_container) return;

    int dt = menuSZK->deltaTime;

    if (Messages.empty()) {
        s_container->text = "";
        return;
    }

    // se nenhuma mensagem atual, mostra a primeira
    if (s_currentIndex < 0 && !Messages.empty()) {
        s_currentIndex = 0;
        s_timer = 0;
        s_container->text = Messages[s_currentIndex].text;
    }

    if (s_currentIndex >= 0 && s_currentIndex < (int)Messages.size()) {
        s_timer += dt;
        auto& msg = Messages[s_currentIndex];

        if (s_timer >= msg.time) {
            s_timer = 0;
            s_currentIndex++;

            if (s_currentIndex >= (int)Messages.size()) {
                // acabou todas
                ClearMessages();
                s_currentIndex = -1;
            } else {
                // próxima
                s_container->text = Messages[s_currentIndex].text;
            }
        }
    }
}

void BottomMessage::AddMessage(const std::string& message, int timeMs)
{
    Messages.push_back({ message, timeMs });
}

void BottomMessage::SetMessage(const std::string& message, int timeMs)
{
    Messages.clear();
    Messages.push_back({ message, timeMs });
    s_timer = 0;
    if (s_container)
        s_container->text = message;
}

void BottomMessage::ClearMessages()
{
    Messages.clear();
    s_timer = 0;
    if (s_container)
        s_container->text = "";
}