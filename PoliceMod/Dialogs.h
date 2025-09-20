#pragma once

#include "pch.h"

struct BasicDialog
{
    std::string title;
    std::string message;
    int time;
};

class Dialogs {
private:
    static std::vector<BasicDialog*> dialogs;
public:
    static void Initialize();
    static void AddDialog(std::string message, int time);
    static void AddDialog(std::string title, std::string message, int time);
    static void AddOfficerDialog(std::string message, int time);
    static void Update();
    static bool HasDialogs() { return !dialogs.empty(); }
};