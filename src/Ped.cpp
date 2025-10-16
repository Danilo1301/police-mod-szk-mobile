#include "Ped.h"

#include "globals.h"
#include "CleoFunctions.h"
#include "Pullover.h"

Ped::Ped(int ref, void* ptr)
{
    this->ref = ref;
    this->ptr = ptr;

    {
        auto widget = menuSZK->CreateWidget(
            200,
            200,
            100,
            modData->GetFileFromMenuSZK("assets/widget_background1.png"),
            modData->GetFile("assets/widgets/widget_pullover.png")
        );
        widget->visible = false;

        WorldWidget w;
        w.widget = widget;
        w.attachToPed = ref;
        WorldWidgets.push_back(w);

        widget->onClick->Add([this]() {
            Pullover::OpenPedMenu(this);
        });

        widgetOptions = widget;
    }
}

Ped::~Ped()
{
    if(widgetOptions)
    {
        widgetOptions->Close();
        widgetOptions = nullptr;
    }
}

void Ped::Update()
{
    PerformAnims();

    auto pedPosition = GetPosition();
    auto playerPosition = *g_playerPosition;

    if(widgetOptions)
    {
        bool widgetVisible = false;

        if(distanceBetweenPoints(pedPosition, playerPosition) <= 5.0f)
        {
            widgetVisible = true;

            if(!IsPerformingAnimation("handsup"))
                widgetVisible = false;
        }

        widgetOptions->visible = widgetVisible;
    }
}

void Ped::SetCanDoHandsup(bool state)
{
    flags.canDoAnimHandsup = state;

    PerformAnims();
}

void Ped::PerformAnims()
{
    if(flags.canDoAnimHandsup)
    {
        if(!IsPerformingAnimation("handsup"))
        {
            menuDebug->AddLine("playing handsup");
            
            CLEAR_ACTOR_TASK(ref);
            PERFORM_ANIMATION_AS_ACTOR(ref, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
        }
    }

    if(flags.canDoAnimCover)
    {
        if(!IsPerformingAnimation("cower"))
        {
            menuDebug->AddLine("playing cower");

            CLEAR_ACTOR_TASK(ref);
            PERFORM_ANIMATION_AS_ACTOR(ref, "cower", "PED", 4.0f, 0, 0, 0, 1, -1);
        }
    }
}

bool Ped::IsPerformingAnimation(const std::string& animName)
{
    return ACTOR_PERFORMING_ANIMATION(ref, animName.c_str());
}

void Ped::ShowBlip(CRGBA color)
{
    flags.showBlip = true;
    flags.blipColor = color;
}

void Ped::HideBlip()
{
    flags.showBlip = false;
}

CVector Ped::GetPosition()
{
    return GetPedPosition(ref);
}