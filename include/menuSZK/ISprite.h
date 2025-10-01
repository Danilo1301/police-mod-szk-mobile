#pragma once

class ISprite {
public:
    bool loaded = false;

    virtual void* GetCSprite2d() = 0;
};