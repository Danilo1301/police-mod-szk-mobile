#include "PoliceBases.h"

std::vector<PoliceBase*> PoliceBases::Bases;

void PoliceBases::Initialize()
{
    auto base = new PoliceBase();
    base->basePosition = CVector(2495.9983, -1744.7815, 13.4679);
    base->leaveCriminalPosition = CVector(2495.9983, -1744.7815, 13.4679);
    base->leaveCriminalCheckpoint = Checkpoints::CreateCheckpoint(base->leaveCriminalPosition);

    Bases.push_back(base);
}

void PoliceBases::Update()
{
    for(auto base : Bases)
    {
        base->Update();
    }
}

void PoliceBases::OnRender()
{
    for(auto base : Bases)
    {
        base->OnRender();
    }
}

void PoliceBases::OnDrawRadar()
{
    for(auto base : Bases)
    {
        base->OnDrawRadar();
    }
}