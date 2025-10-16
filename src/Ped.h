#pragma once

#include "pch.h"

struct ePedFlags
{
    bool hasSurrended = false;
    bool willSurrender = true;
    bool canDoAnimHandsup = false;
    bool canDoAnimCover = false;

    CRGBA blipColor = CRGBA(255, 255, 255);
    bool showBlip = false;
};

class Ped {
private:
    
public:
    int ref;
    void* ptr;

    ePedFlags flags;

    IWidget* widgetOptions = nullptr;

    Ped(int ref, void* ptr);
    ~Ped();

    void Update();

    void SetCanDoHandsup(bool state);
    void PerformAnims();
    bool IsPerformingAnimation(const std::string& animName);

    void ShowBlip(CRGBA color);
    void HideBlip();

    CVector GetPosition();
};