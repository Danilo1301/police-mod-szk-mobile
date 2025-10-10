#include "Criminals.h"

#include "Peds.h"
#include "Vehicles.h"
#include "CleoFunctions.h"

std::vector<Ped*> criminals;

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

void Criminals::Update()
{
    for (auto it = criminals.begin(); it != criminals.end(); )
    {
        Ped* p = *it;

        if (!Peds::IsValid(p))
        {
            // apaga e anda pro próximo
            it = criminals.erase(it);
        }
        else
        {
            ++it; // só avança
        }
    }
}

std::vector<Ped*> Criminals::GetCriminals()
{
    return criminals;
}

std::vector<Vehicle*> Criminals::GetCriminalsVehicles()
{
    std::vector<Vehicle*> vehicles;

    for (auto* ped : criminals)
    {
        int vehicleRef = GetVehiclePedIsUsing(ped->ref);
        auto* vehicle = Vehicles::GetVehicle(vehicleRef);

        if (!vehicle) continue;

        // evita adicionar o mesmo veículo duas vezes
        if (std::find(vehicles.begin(), vehicles.end(), vehicle) == vehicles.end())
        {
            vehicles.push_back(vehicle);
        }
    }

    return vehicles;
}

Ped* Criminals::GetClosestCriminal(CVector fromPosition)
{
    auto criminals = Criminals::GetCriminals();

    if (criminals.empty())
        return nullptr;

    Ped* nearestNotHandsUp = nullptr;
    float bestDistNot = std::numeric_limits<float>::infinity();

    Ped* nearestAny = nullptr;
    float bestDistAny = std::numeric_limits<float>::infinity();

    for (Ped* ped : criminals)
    {
        if (!ped)
            continue;

        float dist = distanceBetweenPoints(fromPosition, ped->GetPosition());

        if (dist < bestDistAny)
        {
            bestDistAny = dist;
            nearestAny = ped;
        }

        if (!ped->IsDoingHandsupAnim() && dist < bestDistNot)
        {
            bestDistNot = dist;
            nearestNotHandsUp = ped;
        }
    }

    // Prioriza quem não está de handsup
    return (nearestNotHandsUp != nullptr) ? nearestNotHandsUp : nearestAny;
}