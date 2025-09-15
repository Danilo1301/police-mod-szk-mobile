#pragma once

#include <functional>
#include <vector>

template<typename... Args>
class IEventListener {
public:
    using Callback = std::function<void(Args...)>;

    virtual ~IEventListener() = default;

    virtual void Add(const Callback& cb) = 0;
    virtual void Emit(Args... args) = 0;
};