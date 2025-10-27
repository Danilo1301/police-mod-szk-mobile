#include "ATMSystem.h"

#include "CleoFunctions.h"
#include "ModelLoader.h"

const int ATM_ID = 2942;
const float OFFSET_TO_FIX_TS = 0.8f;

std::vector<Location*> g_atmPositions;

void ATMSystem::Initialize()
{
    g_atmPositions.push_back(new Location{ CVector(1192.14, -1782.2, 13.2984), CVector(0, 0, 180) });
    g_atmPositions.push_back(new Location{ CVector(1729.3, -1745.94, 13.1297), CVector(0, 0, -90) });

    ModelLoader::AddModelToLoad(ATM_ID);
    ModelLoader::LoadAll([]() {
        CreateObjects();
    });
}

void ATMSystem::CreateObjects()
{
    for(auto location : g_atmPositions)
    {
        auto position = location->position;

        auto objectId = CREATE_OBJECT(ATM_ID, position.x, position.y, position.z - OFFSET_TO_FIX_TS);
        SET_OBJECT_Z_ANGLE(objectId, location->rotation.z);
        SET_OBJECT_PROOFS(objectId, true, true, true, true, true);
    }
}

Location* ATMSystem::GetRandomLocation()
{
    if (g_atmPositions.empty())
        return nullptr;

    // inicializa o gerador de números aleatórios uma vez (pode colocar no construtor)
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    int randomIndex = std::rand() % g_atmPositions.size();

    return g_atmPositions[randomIndex];
}