#include "Vehicles.h"

#include "CleoFunctions.h"

std::map<int, Vehicle*> Vehicles::vehicles;

void Vehicles::AddVehicle(int ref, void* ptr)
{
    if(vehicles.find(ref) != vehicles.end())
    {
        // já existe
        return;
    }

    auto veh = new Vehicle(ref, ptr);
    vehicles[ref] = veh;
}

Vehicle* Vehicles::RegisterVehicle(int ref)
{
    if(vehicles.find(ref) != vehicles.end())
    {
        // já existe
        return NULL;
    }

    auto ptr = menuSZK->GetCVehicleFromRef(ref);

    auto veh = new Vehicle(ref, ptr);
    vehicles[ref] = veh;

    return veh;
}

void Vehicles::RemoveVehicle(int ref)
{
    // verifica se existe
    auto it = vehicles.find(ref);
    if (it == vehicles.end())
        return; // não existe, sai

    // pega o ped
    Vehicle* veh = it->second;

    // remove do map
    vehicles.erase(it);

    // deleta o objeto
    delete veh;
}

bool Vehicles::IsValid(Vehicle* veh)
{
    if (!veh) return false; // ponteiro nulo nunca é válido

    for (const auto& pair : vehicles)
    {
        if (pair.second == veh) // encontrou o ponteiro
        {
            return CAR_DEFINED(veh->ref);
        }
    }

    return false; // não encontrado
}

Vehicle* Vehicles::GetVehicle(int ref)
{
    if(ref < 0) return nullptr;

    auto it = vehicles.find(ref);
    if (it == vehicles.end())
        return nullptr;   // não existe

    return it->second;    // existe, retorna
}

std::map<int, Vehicle*> Vehicles::GetVehiclesMap()
{
    return vehicles;
}

void Vehicles::Update()
{
    auto vehiclesCopy = vehicles;

    // percorre todos os peds no map
    for (auto& pair : vehiclesCopy)
    {
        Vehicle* veh = pair.second;
        if (!veh) continue;

        // chama o update individual do ped
        veh->Update();  
    }
}

std::vector<Vehicle*> Vehicles::GetAllCarsInSphere(CVector position, float radius)
{
    std::vector<Vehicle*> foundVehicles;

    for(auto pair : vehicles)
    {
        auto vehicle = pair.second;

        auto vehiclePos = GetCarPosition(vehicle->ref);
        auto distance = distanceBetweenPoints(position, vehiclePos);

        if(distance <= radius)
        {
            foundVehicles.push_back(vehicle);
        }
    }

    return foundVehicles;
}

Vehicle* Vehicles::GetClosestVehicle(CVector sphereCenter, CVector targetPosition, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(sphereCenter, radius);

    Vehicle* closestCar = NULL;
    double closestDistance = 99999;

    for(size_t i = 0; i < vehicles.size(); i++)
    {
        auto vehicle = vehicles[i];
        auto vehiclePosition = GetCarPosition(vehicle->ref);
        
        auto distance = distanceBetweenPoints(vehiclePosition, targetPosition);

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;
        }
    }

    if(!closestCar) return NULL;

    return closestCar;
}

Vehicle* Vehicles::GetClosestVehicleNotPlayer(CVector sphereCenter, CVector targetPosition, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(sphereCenter, radius);

    Vehicle* closestCar = NULL;
    double closestDistance = 99999;

    for(size_t i = 0; i < vehicles.size(); i++)
    {
        auto vehicle = vehicles[i];

        if(vehicle->IsPlayerInside()) continue;

        auto vehiclePosition = GetCarPosition(vehicle->ref);
        
        auto distance = distanceBetweenPoints(vehiclePosition, targetPosition);

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;
        }
    }

    if(!closestCar) return NULL;

    return closestCar;
}