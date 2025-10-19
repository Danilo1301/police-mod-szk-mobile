#pragma once

#include "pch.h"
#include "AIVehicle.h"

class AIBackupVehicle : public AIVehicle {
protected:
    int criminalRef = 0;
    bool copsAreLeaving = false;
    bool copsAreEntering = false;
    int findCriminalTimer = 0;
public:

    ~AIBackupVehicle() override;

    void Start() override;
    void Update() override;

    void FindCriminal();
    void GotoCriminal();
    bool CriminalExists();
    
    int FindBestCriminal(CVector position);

    void ProcessFindCriminalTimer();
};