#include "RadioWindow.h"

#include "BottomMessage.h"

IContainer* RadioWindow::MainContainer = nullptr;
IContainer* RadioWindow::ScreenContainer = nullptr;

std::map<std::string, RadioScreenGroup> RadioWindow::groups;

std::string RadioWindow::currentGroupId;

int RadioWindow::currentScreenIndex = 0;

void RadioWindow::Initialize()
{
    auto container = menuSZK->CreateContainer("radio");
    container->localOffset = CVector2D(400, 170);
    container->size = CVector2D(600, 1000);
    container->horizontalAlign = HorizontalAlign::Left;
    container->verticalAlign = VerticalAlign::Bottom;
    container->drawBackground = false;
    container->clickThroughMode = ClickThroughMode::ClickThroughThisOnly;
    container->SetBackgroundImage(modData->GetFileFromAssets("radio/radio.png"));
    menuSZK->GetRootContainer()->AddChild(container);

    MainContainer = container;

    {
        auto container = menuSZK->CreateContainer("up");
        container->localOffset = CVector2D(450, 450);
        container->size = CVector2D(100, 100);
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = false;
        container->SetBackgroundImage(modData->GetFileFromAssets("radio/button_up.png"));
        MainContainer->AddChild(container);

        container->onClick->Add([] () {
            PrevScreen();
        });
    }

    {
        auto container = menuSZK->CreateContainer("down");
        container->localOffset = CVector2D(450, 570);
        container->size = CVector2D(100, 100);
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = false;
        container->SetBackgroundImage(modData->GetFileFromAssets("radio/button_down.png"));
        MainContainer->AddChild(container);

        container->onClick->Add([] () {
            NextScreen();
        });
    }

    {
        auto container = menuSZK->CreateContainer("select");
        container->localOffset = CVector2D(20, 530);
        container->size = CVector2D(100, 100);
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = false;
        container->SetBackgroundImage(modData->GetFileFromAssets("radio/button_select.png"));
        MainContainer->AddChild(container);

         container->onClick->Add([] () {
            Select();
        });
    }

    {
        auto container = menuSZK->CreateContainer("close");
        container->localOffset = CVector2D(20, 720);
        container->size = CVector2D(100, 100);
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = false;
        container->SetBackgroundImage(modData->GetFileFromAssets("radio/button_close.png"));
        MainContainer->AddChild(container);

        container->onClick->Add([] () {
            Back();
        });
    }

    {
        auto container = menuSZK->CreateContainer("radio_image");
        container->localOffset = CVector2D(198, 576);
        container->size = CVector2D(180, 170);  
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = true;
        MainContainer->AddChild(container);

        ScreenContainer = container;
    }

    AddGroup("main", "");
    currentGroupId = "main";
    currentScreenIndex = 0;

    AddScreen("main", "cancel_chase", "cancel_chase.png");
    AddScreen("main", "test_menu", "test_menu.png");

    AddGroup("test", "main");

    AddScreen("test", "test1", "test.png");
    AddScreen("test", "test2", "test.png");
    AddScreen("test", "test3", "test.png");

    UpdateScreenContainer();
}

void RadioWindow::Toggle()
{
    MainContainer->isVisible = !MainContainer->isVisible;
}

void RadioWindow::OnSelect(std::string id)
{
    if(id == "cancel_chase")
    {
        Toggle();
        return;
    }

    if(id == "test_menu")
    {
        SwitchToGroup("test");
        return;
    }

    BottomMessage::SetMessage("~r~No action defined", 1000);
}