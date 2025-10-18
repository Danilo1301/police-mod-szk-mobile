#pragma once

#include "pch.h"

class ScriptTask {
private:
    int timePassedToTimeout = 0;
    int timeout = -1;
public:
    std::string name;
    bool completed = false;
    bool deleteOnComplete = true;
    std::function<void()> onBegin;
    std::function<bool()> onExecute;
    std::function<void()> onComplete;

    ScriptTask(std::string name);

    void Start();
    void SetStartAgainIfTimeout(int timeOut);

    static void MakeVehicleLeave(int vehicleRef);

    static void CreateTest();
};