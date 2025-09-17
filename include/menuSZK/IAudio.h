#pragma once

class IAudio {
public:
    virtual bool StreamHasLoaded() = 0;
    virtual void Play() = 0;
    virtual void PlayAndDestroy() = 0;
    virtual void Destroy() = 0;
    virtual bool HasEnded() = 0;
    virtual bool IsPlaying() = 0;
    virtual int GetState() = 0;
};