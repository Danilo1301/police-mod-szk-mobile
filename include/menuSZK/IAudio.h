#pragma once

struct IAudio {
public:
    virtual ~IAudio() = default;

    virtual void Play() = 0;
    virtual void Stop() = 0;
    virtual void SetLoop(bool loop) = 0;
    virtual bool Finished() = 0;
    virtual bool Loaded() = 0;
    virtual bool Is3D() = 0;
    virtual void AttachToCPlaceable(void* ptr) = 0;
    virtual void SetVolume(float volume) = 0;
};