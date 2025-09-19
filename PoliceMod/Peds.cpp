#include "Peds.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

std::map<int, Ped*> Peds::peds;

void Peds::AddPed(int ref, void* ptr)
{
    if(peds.find(ref) != peds.end())
    {
        // já existe
        return;
    }

    auto ped = new Ped(ref, ptr);
    peds[ref] = ped;
}

Ped* Peds::RegisterPed(int ref)
{
    if(peds.find(ref) != peds.end())
    {
        // já existe
        return NULL;
    }

    auto ptr = menuSZK->GetCPedFromRef(ref);

    auto ped = new Ped(ref, ptr);
    peds[ref] = ped;

    return ped;
}

void Peds::RemovePed(int ref)
{
    auto ped = peds[ref];
    peds.erase(ref);

    delete ped;
}

Ped* Peds::GetPed(int ref)
{
    auto it = peds.find(ref);
    if (it == peds.end())
        return nullptr;   // não existe

    return it->second;    // existe, retorna
}

void Peds::Update()
{
    // percorre todos os peds no map
    for (auto& pair : peds)
    {
        Ped* ped = pair.second;
        if (!ped) continue;

        // chama o update individual do ped
        ped->Update();  
    }
}