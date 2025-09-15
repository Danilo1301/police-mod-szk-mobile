#include "Peds.h"

std::map<int, Ped*> Peds::peds;

void Peds::AddPed(int ref, void* ptr)
{
    auto ped = new Ped(ref, ptr);
    peds[ref] = ped;
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