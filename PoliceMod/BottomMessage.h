#pragma once

#include "pch.h"

struct BasicBMessage
{
    std::string title;
    std::string message;
    int time;
};

class BottomMessage {
private:
    static std::vector<BasicBMessage*> messages;
public:
    static void Initialize();

    static void AddMessage(std::string title, std::string message, int time);
    static void SetMessage(std::string title, std::string message, int time);
    static void SetMessage(std::string message, int time);
    static void AddOfficerDialog(std::string message, int time);

    static void Update();
    static bool HasMessages() { return !messages.empty(); }
};