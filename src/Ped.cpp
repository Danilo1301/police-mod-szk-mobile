#include "Ped.h"

#include "CleoFunctions.h"
#include "Pullover.h"
#include "Peds.h"
#include "WorldWidgets.h"

#include "BottomMessage.h"

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

    
    wasAlive = ACTOR_HEALTH(ref) > 0;
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

        if(distanceBetweenPoints(pedPosition, playerPosition) < 5.0f)
        {
            widgetVisible = IsPerformingAnimation("handsup") || flags.canDoAnimHandsup;
        }

        widgetOptions->visible = widgetVisible;
    }

    auto health = ACTOR_HEALTH(ref);

    if(health <= 0 && wasAlive)
    {
        wasAlive = false;

        auto ref = this->ref;

        WAIT(6000, [ref]() {

            if(ref == GetPlayerActor()) return;

            if(!ACTOR_DEFINED(ref)) return;

            auto ped = Peds::GetPed(ref);
            auto modelId = GET_ACTOR_MODEL(ref);
            auto position = ped->GetPosition();

            auto newPedRef = CREATE_ACTOR_PEDTYPE(PedType::CivMale, modelId, position.x, position.y, position.z);
            auto newPed = Peds::RegisterPed(newPedRef);
            newPed->flags.isInconcious = true;

            //PERFORM_ANIMATION_AS_ACTOR(newPedRef, "FALL_back", "PED", 10.0f, 0, 0, 0, 1, -1);
            PERFORM_ANIMATION_AS_ACTOR(newPedRef, "crckdeth2", "CRACK", 10.0f, 0, 0, 0, 1, -1);

            DESTROY_ACTOR(ref);
            Peds::RemovePed(ref);

            auto cped = (CPed*)newPed->ptr;
            cped->m_matrix->at = CVector(position.x, position.y, position.z - 0.3f);
        });
    }
}

void Ped::SetCanDoHandsup(bool state)
{
    flags.canDoAnimHandsup = state;

    PerformAnims();

    if(state == false)
    {
        ClearPedAnimations(ref);
    }
}

void Ped::PerformAnims()
{
    if(flags.canDoAnimHandsup)
    {
        if(!IsPerformingAnimation("handsup"))
        {
            CLEAR_ACTOR_TASK(ref);
            PERFORM_ANIMATION_AS_ACTOR(ref, "handsup", "PED", 4.0f, 0, 0, 0, 1, -1);
        }
    }

    if(flags.canDoAnimCover)
    {
        if(!IsPerformingAnimation("cower"))
        {
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