#include "PoliceBases.h"

std::vector<PoliceBase*> PoliceBases::Bases;

void PoliceBases::Initialize()
{
    auto base = new PoliceBase();
    base->basePosition = CVector(1539.3256, -1688.3606, 13.5469);
    base->leaveCriminalPosition = CVector(1539.3256, -1688.3606, 13.5469);

    base->leaveCriminalCheckpoint->position = base->leaveCriminalPosition;

    Bases.push_back(base);
}

void PoliceBases::Update()
{
    for(auto base : Bases)
    {
        base->Update();
    }
}

void PoliceBases::OnPostDrawRadar()
{
    for(auto base : Bases)
    {
        base->OnPostDrawRadar();
    }
}

void PoliceBases::OnDraw()
{
    for(auto base : Bases)
    {
        base->OnDraw();
    }
}