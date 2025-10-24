#pragma once

#include "menuSZK/IEventListener.h"

template<typename... Args>
class EventListener : public IEventListener<Args...> {
public:
    using Callback = typename IEventListener<Args...>::Callback;

    void Add(const Callback& cb) override {
        callbacks.push_back(cb);
    }

    void Emit(Args... args) override {
        for (auto& cb : callbacks) {
            cb(args...);
        }
    }

private:
    std::vector<Callback> callbacks;
};