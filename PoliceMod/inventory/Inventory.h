#pragma once

#include "pch.h"

// Representa um item de inventário
struct InventoryItem
{
    std::string id;
    std::string name = "Item sem nome";
    std::string description = "Descricao";
    int quantity = 0;
    float spawnChance = 1.0f; // 0.0 a 1.0

    InventoryItem(std::string n)
        : id(n) {}
};

// Gerenciador de inventário
class Inventory
{
private:
    std::vector<InventoryItem*> items;
public:
    void AddItem(InventoryItem* item);
    void RemoveItem(InventoryItem* item);
};