#include "Objectives.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

std::string Objectives::currentObjective = "";

ILabel* objectiveLabel = NULL;

void Objectives::Initialize()
{
    auto label = objectiveLabel = menuSZK->CreateLabel();
    label->textHorizontalAlign = HorizontalAlign::Middle;
    label->textVerticalAlign = VerticalAlign::Middle;

    auto container = label->GetContainer();
    container->horizontalAlign = HorizontalAlign::Middle;
    container->verticalAlign = VerticalAlign::Top;
    container->localOffset = CVector2D(0, 80);
    container->drawBackground = false; //ativar pra testar

    menuSZK->GetScreenContainer()->AddChild(label->GetContainer());
}

void Objectives::SetObjective(std::string objective)
{
    currentObjective = objective;
}

void Objectives::ClearObjective()
{
    currentObjective = "";
}

void Objectives::Update()
{
    bool hasObjective = !currentObjective.empty();

    if(objectiveLabel)
    {
        objectiveLabel->GetContainer()->isVisible = hasObjective;

        if(hasObjective)
        {
            objectiveLabel->text = "~y~" + currentObjective;
        }
    }
}