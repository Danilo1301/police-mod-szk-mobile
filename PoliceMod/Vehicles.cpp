#include "Vehicles.h"

#include "CleoFunctions.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

std::map<int, Vehicle*> Vehicles::vehicles;

void Vehicles::AddVehicle(int ref, void* ptr)
{
    if(vehicles.find(ref) != vehicles.end())
    {
        // já existe
        return;
    }

    auto vehicle = new Vehicle(ref, ptr);
    vehicles[ref] = vehicle;
}

Vehicle* Vehicles::RegisterVehicle(int ref)
{
    if(vehicles.find(ref) != vehicles.end())
    {
        // já existe
        return NULL;
    }

    auto ptr = menuSZK->GetCVehicleFromRef(ref);

    auto vehicle = new Vehicle(ref, ptr);
    vehicles[ref] = vehicle;

    return vehicle;
}

void Vehicles::RemoveVehicle(int ref)
{
    auto vehicle = vehicles[ref];
    vehicles.erase(ref);

    delete vehicle;
}

Vehicle* Vehicles::GetVehicle(int ref)
{
    auto it = vehicles.find(ref);
    if (it == vehicles.end())
        return nullptr;  // não existe

    return it->second;   // existe, retorna o Vehicle*
}

Vehicle* Vehicles::GetVehicle(void* ptr)
{
    for(auto pair : vehicles)
    {
        auto vehicle = pair.second;

        if(vehicle->ptr == ptr)
        {
            return vehicle;
        }
    }

    return NULL;
}

bool Vehicles::IsValid(Vehicle* vehicle)
{
    if (!vehicle) return false; // ponteiro nulo nunca é válido

    for (const auto& pair : vehicles)
    {
        if (pair.second == vehicle) // encontrou o ponteiro
            return true;
    }

    return false; // não encontrado
}

void Vehicles::Update()
{
    for (auto& pair : vehicles)
    {
        Vehicle* vehicle = pair.second;
        if (!vehicle) continue;

        vehicle->Update();  
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

Vehicle* Vehicles::GetClosestVehicle(CVector position, float radius)
{
    std::vector<Vehicle*> vehicles = GetAllCarsInSphere(position, radius);

    Vehicle* closestCar = NULL;
    double closestDistance = 99999;

    auto playerActor = CleoFunctions::GET_PLAYER_ACTOR(0);

    float playerX = 0.0f, playerY = 0.0f, playerZ = 0.0f;
    CleoFunctions::GET_CHAR_COORDINATES(playerActor, &playerX, &playerY, &playerZ);

    for(size_t i = 0; i < vehicles.size(); i++)
    {
        auto vehicle = vehicles[i];
        auto vehiclePosition = GetCarPosition(vehicle->ref);
        
        auto distance = distanceBetweenPoints(vehiclePosition, CVector(playerX, playerY, playerZ));

        if(distance < closestDistance)
        {
            closestDistance = distance;
            closestCar = vehicle;
        }
    }

    if(!closestCar) return NULL;

    return closestCar;
}
