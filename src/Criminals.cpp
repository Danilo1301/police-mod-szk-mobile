#include "Criminals.h"

#include "Peds.h"
#include "CleoFunctions.h"

std::vector<Ped*> criminals;

void Criminals::Update()
{
    
}

void Criminals::AddCriminal(Ped* ped)
{
    if(IsCriminal(ped)) return;

    criminals.push_back(ped);
}

void Criminals::RemoveCriminal(Ped* ped)
{
    if(!IsCriminal(ped)) return;

    auto it = std::find(criminals.begin(), criminals.end(), ped);

    criminals.erase(it);
}

bool Criminals::IsCriminal(Ped* ped)
{
    if(ped == nullptr) return false;

    auto it = std::find(criminals.begin(), criminals.end(), ped);

    return it != criminals.end();
}