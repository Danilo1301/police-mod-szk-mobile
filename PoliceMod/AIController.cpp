#include "AIController.h"

#include "Vehicles.h"

std::map<Vehicle*, VehicleAI*> AIController::vehicleAIs;

void AIController::AddAIToVehicle(Vehicle* vehicle, VehicleAI* ai)
{
    if (!vehicle || !ai) return; // checagem básica

    ai->vehicle = vehicle;

    // adiciona ou atualiza
    vehicleAIs[vehicle] = ai;
}

void AIController::RemoveAIsFromVehicle(Vehicle* vehicle)
{
    if (!vehicle) return;

    auto it = vehicleAIs.find(vehicle);
    if (it != vehicleAIs.end())
    {
        VehicleAI* ai = it->second;

        if (ai != nullptr)
            delete ai; // desaloca memória da AI

        vehicleAIs.erase(it); // remove do map
    }
}

void AIController::Update()
{
    // cópia rasa do mapa (shallow copy dos pares)
    auto cachedAIs = vehicleAIs;

    for (const auto& [vehicle, ai] : cachedAIs)
    {
        // se ponteiro nulo ou veículo inválido -> remove do mapa original (se existir) e pula
        if (!Vehicles::IsValid(vehicle))
        {
            auto itOrig = vehicleAIs.find(vehicle);
            if (itOrig != vehicleAIs.end())
            {
                RemoveAIsFromVehicle(vehicle);
            }
            continue;
        }

        // só atualiza se o par ainda existir no mapa original e apontar para o mesmo AI
        auto itOrig = vehicleAIs.find(vehicle);
        if (itOrig != vehicleAIs.end() && itOrig->second == ai)
        {
            ai->Update(); // dentro daqui o AI pode chamar RemoveAIsFromVehicle(...)
        }
    }
}