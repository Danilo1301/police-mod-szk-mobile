#include "Checkpoint.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#include "CleoFunctions.h"

void Checkpoint::Update()
{
    bool canCreate = true;

    auto distanceToPlayer = DistanceFromPed(GetPlayerActor(), position);
    if(distanceToPlayer > 100)
    {
        canCreate = false;
    }

    if(canCreate)
    {
        if(checkpoint != NO_CHECKPOINT)
        {
            auto distance = distanceBetweenPoints(position, lastPosition);

            if(distance > 0.5f)
            {
                DestroyCheckpoint();
            }
        }

        if(checkpoint == NO_CHECKPOINT)
        {
            checkpoint = CREATE_SPHERE(position.x, position.y, position.z, radius);
            lastPosition = position;
        }
    } else {
        if(checkpoint != NO_CHECKPOINT)
        {
            DestroyCheckpoint();
        }
    }
}

bool Checkpoint::IsInRange(CVector point)
{
    return distanceBetweenPoints(lastPosition, point) <= (radius - 0.8f);
}

void Checkpoint::DestroyCheckpoint()
{
    if(checkpoint == NO_CHECKPOINT) return;

    DESTROY_SPHERE(checkpoint);
    checkpoint = NO_CHECKPOINT;
}

// --------------------------------------------

std::vector<Checkpoint*> Checkpoints::AllCheckpoints;

void Checkpoints::Update()
{
    for(auto checkpoint : AllCheckpoints)
    {
        checkpoint->Update();
    }
}

Checkpoint* Checkpoints::CreateCheckpoint(CVector position)
{
    auto checkpoint = new Checkpoint();
    checkpoint->position = position;

    AllCheckpoints.push_back(checkpoint);

    debug->AddLine("Checkpoint created");

    return checkpoint;
}

void Checkpoints::DestroyCheckpoint(Checkpoint* checkpoint)
{
    if (!checkpoint)
        return;

    checkpoint->DestroyCheckpoint();

    // Remove do vetor
    auto it = std::find(AllCheckpoints.begin(), AllCheckpoints.end(), checkpoint);
    if (it != AllCheckpoints.end())
        AllCheckpoints.erase(it);

    // Libera memória
    delete checkpoint;

    debug->AddLine("Checkpoint destroyed");
}