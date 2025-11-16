#pragma once

#include "IEventListener.h"

class IWindowItemOptions {
public:
    IEventListener<>* onOptionChange = 0;

    virtual void AddOption(std::string text, int value) = 0;
    virtual int GetCurrentOptionValue() = 0;
    virtual void SetOptionIndex(int index) = 0;
};