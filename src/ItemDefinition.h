#pragma once

#include "pch.h"

struct ItemDefinition {
    std::string name;
    std::string description;
    int maxStack = 1;
    bool isIllegal = false;
    double chance = 1.00;

    ItemDefinition() {};
    ItemDefinition(const std::string& name, const std::string& description, int maxStack = 1)
        : name(name), description(description), maxStack(maxStack) {}
};