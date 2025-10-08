#include "RadioWindow.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "BackupUnits.h"
#include "BottomMessage.h"

bool initialized = false;
IContainer* mainContainer;
int currentPageIndex = 0;
std::string currentPageId = "";

std::unordered_map<std::string, RadioPage*> RadioWindow::pages;

void RadioWindow::Initialize()
{
    if(initialized) return;
    initialized = true;

    auto container = mainContainer = menuSZK->CreateContainer();
    container->horizontalAlign = HorizontalAlign::Left;
    container->verticalAlign = VerticalAlign::Bottom;
    container->localOffset = CVector2D(400, -10);
    container->size = CVector2D(1100, 600);
    container->drawBackground = false;
    container->isVisible = false;
    //container->canClickThrough = true;
    container->SetBackgroundTexture(getPathFromAssets("radio/bg.png"));
    menuSZK->GetScreenContainer()->AddChild(container);

    {
        auto button = menuSZK->CreateContainer();
        button->localOffset = CVector2D(-300, 30);
        button->size = CVector2D(80, 110);
        button->horizontalAlign = HorizontalAlign::Middle;
        button->verticalAlign = VerticalAlign::Middle;
        button->SetBackgroundTexture(getPathFromMenuAssets("left.png"));
        container->AddChild(button);

        button->onClick->Add([](IContainer*) {
            GotoPageIndex(currentPageIndex - 1);
        });
    }

    {
        auto button = menuSZK->CreateContainer();
        button->localOffset = CVector2D(460, 30);
        button->size = CVector2D(80, 110);
        button->horizontalAlign = HorizontalAlign::Middle;
        button->verticalAlign = VerticalAlign::Middle;
        button->SetBackgroundTexture(getPathFromMenuAssets("right.png"));
        container->AddChild(button);

        button->onClick->Add([](IContainer*) {
            GotoPageIndex(currentPageIndex + 1);
        });
    }

    {
        // main page

        auto page = new RadioPage("main");    
        page->buttons.push_back(new RadioButton("main.close", "back"));
        page->buttons.push_back(new RadioButton("main.qth", "qth"));
        page->buttons.push_back(new RadioButton("main.car_options", "car_options"));
        page->buttons.push_back(new RadioButton("main.test", "test"));

        pages[page->id] = page;

        currentPageId = page->id;
    }
    
    {
        // main page

        auto page = new RadioPage("test");    
        page->buttons.push_back(new RadioButton("test.back", "back"));
        page->buttons.push_back(new RadioButton("test", "test"));
        page->buttons.push_back(new RadioButton("test", "test"));
        page->buttons.push_back(new RadioButton("test", "test"));
        page->buttons.push_back(new RadioButton("test", "test"));
        page->buttons.push_back(new RadioButton("test", "test"));
        page->buttons.push_back(new RadioButton("test", "test"));

        pages[page->id] = page;

    }

    auto buttons = GetAllButtons();

    for(auto button : buttons)
    {
        auto container = button->container = menuSZK->CreateContainer();
        container->localOffset = CVector2D(0, 0);
        container->size = CVector2D(300, 100);
        container->horizontalAlign = HorizontalAlign::Left;
        container->verticalAlign = VerticalAlign::Top;
        container->drawBackground = false;
        container->SetBackgroundTexture(getPathFromAssets("radio/buttons/" + button->image + ".png"));

        mainContainer->AddChild(container);

        container->onClick->Add([button](IContainer*) {
            OnClickButton(button->id);
        });
    }

    HideButtons();
}

void RadioWindow::Toggle()
{
    mainContainer->isVisible = !mainContainer->isVisible;

    ShowButtons();
}

void RadioWindow::ShowButtons()
{
    HideButtons();

    auto it = RadioWindow::pages.find(currentPageId);
    if (it == RadioWindow::pages.end())
    {   
        return;
    }

    auto& page = RadioWindow::pages[currentPageId];
    auto& buttons = page->buttons;

    // layout configs
    const float startX = 320;
    const float startY = 160;
    const float spacingX = 320;
    const float spacingY = 120;

    const int maxPerPage = 6;
    const int rows = 3;

    // 2️⃣ calcular início e fim da subpágina
    int start = currentPageIndex * maxPerPage;
    if (start >= (int)buttons.size()) return;

    int end = std::min(start + maxPerPage, (int)buttons.size());

    // 3️⃣ mostrar apenas os botões dessa subpágina
    for (int i = start; i < end; i++)
    {
        auto& button = buttons[i];

        int localIndex = i - start;
        int row = localIndex % rows; // linha 0..2
        int col = localIndex / rows; // coluna 0..1

        float x = startX + col * spacingX;
        float y = startY + row * spacingY;

        button->Show(x, y);
    }
}

void RadioWindow::HideButtons()
{
    for (auto& kv : RadioWindow::pages)
    {
        RadioPage* page = kv.second;

        for (auto& btn : page->buttons) {
            btn->Hide();
        }
    }
}

std::vector<RadioButton*> RadioWindow::GetAllButtons()
{
    std::vector<RadioButton*> all;

    for (auto& kv : RadioWindow::pages) {
        RadioPage* page = kv.second;
        for (auto& btn : page->buttons) {
            all.push_back(btn); // copia
        }
    }

    return all;
}

void RadioWindow::GotoPageIndex(int pageIndex)
{
    auto pageId = currentPageId;

    auto it = RadioWindow::pages.find(pageId);
    if (it == RadioWindow::pages.end())
        return; // página não existe

    auto& page = it->second;
    int totalButtons = (int)page->buttons.size();
    const int maxPerPage = 6;

    // calcula o número máximo de subpáginas
    int maxPageIndex = (totalButtons + maxPerPage - 1) / maxPerPage - 1; 
    if (maxPageIndex < 0) maxPageIndex = 0;

    // clamp o pageIndex entre 0 e maxPageIndex
    if (pageIndex < 0) pageIndex = 0;
    if (pageIndex > maxPageIndex) pageIndex = maxPageIndex;

    currentPageIndex = pageIndex;

    // mostra os botões da subpágina
    ShowButtons();
}

void RadioWindow::GotoPageId(std::string pageId)
{
    currentPageId = pageId;
    currentPageIndex = 0;

    ShowButtons();
}

void RadioWindow::OnClickButton(std::string id)
{
    if(id == "main.test")
    {
        GotoPageId("test");
    }
    if(id == "main.close")
    {
        Toggle();
    }
    if(id == "main.qth")
    {
        BackupUnits::SendQTH();
        Toggle();
    }
    if(id == "main.car_options")
    {
        Toggle();

        Vehicle::CanShowWidgetAnyTime = !Vehicle::CanShowWidgetAnyTime;
        if(Vehicle::CanShowWidgetAnyTime)
        {
            BottomMessage::SetMessage("~g~Ativado ~w~botao de opcoes dos veiculos", 3000);
        } else {
            BottomMessage::SetMessage("~r~Desativado ~w~botao de opcoes dos veiculos", 3000);
        }
    }
    if(id == "test.back")
    {
        GotoPageId("main");
    }
}