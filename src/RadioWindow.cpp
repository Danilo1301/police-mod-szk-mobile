#include "RadioWindow.h"

#include "BottomMessage.h"
#include "Chase.h"
#include "CleoFunctions.h"
#include "BackupUnits.h"
#include "Callouts.h"
#include "TestWindow.h"
#include "ModelLoader.h"

IContainer* RadioWindow::MainContainer = nullptr;
IContainer* RadioWindow::ScreenContainer = nullptr;

bool RadioWindow::m_cancelServices = false;

std::map<std::string, RadioScreenGroup> RadioWindow::groups;

std::string RadioWindow::currentGroupId;

int RadioWindow::currentScreenIndex = 0;

bool g_isWaitingForRadio = false;

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

    AddScreen("main", "send_qth", "send_qth.png");
    AddScreen("main", "callout_menu", "callout_menu.png");
    AddScreen("main", "cancel_chase", "cancel_chase.png");
    AddScreen("main", "cancel_services", "cancel_services.png");
    AddScreen("main", "call_backup", "call_backup.png");
    AddScreen("main", "call_medic", "call_medic.png");
    AddScreen("main", "test_menu", "test_menu.png");

    // AddGroup("test", "main");

    // AddScreen("test", "test1", "test.png");
    // AddScreen("test", "test2", "test.png");
    // AddScreen("test", "test3", "test.png");

    AddGroup("callout", "main");

    AddScreen("callout", "callout_accept", "callout_accept.png");

    UpdateScreenContainer();
    
    MainContainer->isVisible = false;
}

void RadioWindow::Toggle()
{
    bool visible = !MainContainer->isVisible;

    MainContainer->isVisible = visible;

    if(visible)
    {
        ToggleRadioAnim(true);
    } else {
        if(g_isWaitingForRadio) return;
        ToggleRadioAnim(false);
    }
}

void RadioWindow::OnSelect(std::string id)
{
    if(id == "send_qth")
    {
        ToggleRadioAnimOff(5000);
        Toggle();
        BackupUnits::SendQTH();
        return;
    }

    if(id == "callout_menu")
    {
        SwitchToGroup("callout");
        return;
    }

    if(id == "cancel_chase")
    {
        Toggle();
        Chase::AbortChase();
        return;
    }

    if(id == "cancel_services")
    {
        m_cancelServices = true;
        Toggle();

        WAIT(1000, []() {
            m_cancelServices = false;
        });

        return;
    }

    if(id == "call_backup")
    {
        Toggle();
        BackupUnits::OpenSpawnBackupMenu();
        return;
    }

    if(id == "call_medic")
    {
        ToggleRadioAnimOff(5000);
        Toggle();

        auto playerPosition = GetPlayerPosition();

        WAIT(5000, [playerPosition]() {
            BackupUnits::SpawnMedicUnit(playerPosition);
        });

        return;
    }

    if(id == "test_menu")
    {
        Toggle();
        TestWindow::OpenWindow();
        return;
    }

    if(id == "callout_accept")
    {
        if(Callouts::HasCalloutToAccept())
        {
            Toggle();
            Callouts::AcceptCallout();
            return;
        }

        BottomMessage::SetMessage("~r~There are no active callouts", 3000);

        return;
    }

    BottomMessage::SetMessage("~r~No action defined", 1000);
}

int g_radioObject = 0;

void RadioWindow::ToggleRadioAnim(bool state)
{
    if(state == false && g_isWaitingForRadio)
    {
        return;
    }

    if(g_radioObject != 0)
    {
        DESTROY_OBJECT(g_radioObject);
        g_radioObject = 0;
    }

    if(state)
    {
        int radioModelId = 321;

        ModelLoader::AddModelToLoad(radioModelId);
        ModelLoader::LoadAll([radioModelId]() {
            g_radioObject = CREATE_OBJECT(radioModelId, 0, 0, 0);
            ATTACH_TO_OBJECT_AND_PERFORM_ANIMATION(GetPlayerActor(), g_radioObject, 0, 0, 0, 6, 16, "PHONE_TALK", "PED", 1);
        });
    }
}

void RadioWindow::ToggleRadioAnimOff(int timeMs)
{
    g_isWaitingForRadio = true;
    
    WAIT(timeMs, []() {
        g_isWaitingForRadio = false;
        ToggleRadioAnim(false);
    });
}