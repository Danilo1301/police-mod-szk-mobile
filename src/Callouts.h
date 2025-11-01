#pragma once

#include "pch.h"

#define NO_CALLOUT -1
#define CALLOUT_ATM 0

struct Callout {
    
};

class Callouts {
public:
    static void Update();

    static void TryBroadcastCallout();
    static void BroadcastRandomCallout();

    static void AcceptCallout();

    static void OnBeginCallout(int id);

    static bool HasCalloutToAccept();
    static bool IsBroacastingCallout();
    static bool IsInCallout();
};