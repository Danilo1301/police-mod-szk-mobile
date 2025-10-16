#include "BottomMessage.h"

void BottomMessage::Initialize()
{
    auto container = menuSZK->CreateContainer();
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Bottom;
    container->localOffset = CVector2D(0, -100);

    menuSZK->GetRootContainer()->AddChild(container);
}