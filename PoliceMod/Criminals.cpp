#include "Criminals.h"

#include "Peds.h"
#include "Vehicles.h"
#include "CleoFunctions.h"

std::vector<Ped*> criminals;

void Criminals::AddCriminal(Ped* ped)
{
    criminals.push_back(ped);
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