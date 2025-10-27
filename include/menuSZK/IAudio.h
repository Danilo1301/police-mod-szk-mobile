#pragma once

struct IAudio {
public:
    virtual ~IAudio() = default;

    virtual void Play() = 0;
    virtual void SetLoop(bool loop) = 0;
    virtual bool Finished() = 0;
    virtual bool Loaded() = 0;
};