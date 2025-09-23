#include "Inventory.h"


void Inventory::AddItem(InventoryItem* item)
{
    items.push_back(item);
}

void Inventory::RemoveItem(InventoryItem* item)
{
    auto it = std::find(items.begin(), items.end(), item);

    if (it != items.end()) // achou o item
    {
        items.erase(it);
        delete *it; // ⚠️ só se o item foi criado com new!
    }
}