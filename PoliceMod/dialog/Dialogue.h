#pragma once

#include "pch.h"

struct DialogueOption {
    std::string text;    // O que aparece para o jogador
    std::string nextId; 
    bool used = false;
};

struct DialogueNode {
    std::string id;
    std::string npcText;                 // O que o NPC fala
    std::string nextId = "";
    std::vector<DialogueOption> options; // Escolhas do jogador
};

struct DialogueVariables
{
    bool isArmed = false;
    bool isDocummentExpired = false;
    bool hasCNH = false;
};

class Dialogue {
private:
    std::vector<DialogueNode> nodes;
public:
    DialogueVariables variables;

    void AddNode(const DialogueNode& node)
    {
        nodes.push_back(node);
    }

    DialogueNode* FindNode(std::string id) {
        for (auto& n : nodes) {
            if (n.id == id) return &n;
        }
        return nullptr;
    }
};