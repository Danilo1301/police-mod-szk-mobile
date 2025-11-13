#pragma once

#include "../pch.h"

#include "CalloutBase.h"

class CalloutRegistry {
private:
    static inline std::vector<std::unique_ptr<CalloutBase>> s_callouts;
public:
    static void Register(CalloutBase* callout) {
        s_callouts.emplace_back(callout);
    }

    static CalloutBase* GetRandom() {
        if (s_callouts.empty()) return nullptr;
        int idx = rand() % s_callouts.size();
        return s_callouts[idx].get();
    }

    static const std::vector<std::unique_ptr<CalloutBase>>& GetAll() {
        return s_callouts;
    }
};