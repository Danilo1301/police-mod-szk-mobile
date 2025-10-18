#include "TopMessage.h"

IContainer* t_container = nullptr;
std::string t_message = "";

void TopMessage::Initialize()
{
    auto container = menuSZK->CreateContainer("topMessage");
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Top;
    container->localOffset = CVector2D(0, 150);
    container->clickThroughMode = ClickThroughMode::ClickThroughThisAndChildren;
    container->drawBackground = false;

    container->text = "Aqui vai ficar a mensagme";
    container->textHorizontalAlign = HorizontalAlign::Middle;
    container->textVerticalAlign = VerticalAlign::Middle;

    auto font = &container->textFont;
    font->fontSize = 3.0f;
    font->align = eFontAlignment::ALIGN_CENTER;

    menuSZK->GetRootContainer()->AddChild(container);

    t_container = container;
}

void TopMessage::Update()
{
    if (!t_container) return;

    if (t_message.empty()) {
        t_container->text = "";
        return;
    }

    t_container->text = t_message;
}

void TopMessage::SetMessage(const std::string& message)
{
    t_message = message;
}

void TopMessage::ClearMessage()
{
    t_message = "";
}