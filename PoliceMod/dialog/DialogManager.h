#pragma once

#include "pch.h"

#include "Dialogue.h"

class DialogManager {
private:
    static std::string currentId;
    static std::stack<std::string> history;
public:
    static std::vector<Dialogue> loadedDialogs;
    static Dialogue* currentDialogue;

    static void Initialize();
    static void Update();

    static void BeginDialogue(Dialogue* dialogue);
    static void EndDialogue();

    static void OnClickAnywhere();
    static void ShowOptionsForNode(DialogueNode* node);

    static bool ContainsInHistory(const std::string& id)
    {
        auto temp = history; // copia a stack
        while (!temp.empty())
        {
            if (temp.top() == id)
                return true;
            temp.pop();
        }
        return false;
    };

    static void LoadDialogs();
    static Dialogue LoadDialogFromFile(std::string src);
};