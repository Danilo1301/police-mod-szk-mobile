#include "InventoryItemManager.h"
#include "IniReaderWriter.hpp"

std::unordered_map<std::string, std::shared_ptr<ItemDefinition>> InventoryItemManager::itemDefinitions;

void InventoryItemManager::Initialize()
{
    LoadItems();

    //itemDefinitions["apple"] = std::make_shared<ItemDefinition>("Apple", "A fresh red apple", 10);
    //itemDefinitions["keys"] = std::make_shared<ItemDefinition>("Keys", "House keys", 1);
}

void InventoryItemManager::LoadItems()
{
    auto itemsFolder = modData->GetFile("data/items"); // caminho da pasta
    IniReaderWriter ini;

    for (const auto& entry : std::filesystem::directory_iterator(itemsFolder))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ini") continue; // só processa .ini

        std::string path = entry.path().string();
        std::string id = entry.path().stem().string(); // nome do arquivo sem extensão -> id

        if (!ini.LoadFromFile(path)) continue; // ignora arquivos que falhem no parse

        std::string name = ini.Get("item", "name", id);
        std::string description = ini.Get("item", "description", "No description");
        double chance = ini.GetDouble("item", "chance", 1.00);
        int maxAmount = ini.GetInt("item", "max_amount", 1);
        bool isIllegal = ini.GetBool("item", "is_illegal", false);
        std::string specialRole = ini.Get("item", "special_role", "");
        bool isStolen = ini.GetBool("item", "is_stolen", false);

        // Cria shared_ptr e adiciona ao map
        auto itemDef = std::make_shared<ItemDefinition>();
        itemDef->name = name;
        itemDef->description = description;
        itemDef->chance = chance;
        itemDef->maxStack = maxAmount;
        itemDef->isIllegal = isIllegal;
        itemDef->specialRole = specialRole;
        itemDef->isStolen = isStolen;

        itemDefinitions[id] = itemDef;
    }
}