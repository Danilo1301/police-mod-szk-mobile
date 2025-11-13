#pragma once

#include "../AudioVariationGroup.h"

class CalloutBase {
public:
    virtual ~CalloutBase() = default;

    virtual const char* GetBroadcastMessage() = 0;
    virtual AudioVariationGroup* GetBroadcastAudio() = 0;

    virtual void OnAccept() = 0;
};