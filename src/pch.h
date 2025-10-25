#pragma once

#include "ModData.h"
#include "Logger.h"

// CLEO 2.0.1.3
#include "cleo.h"
extern cleo_ifs_t* cleo;

// SAUtils 1.6
#include "isautils.h"
extern ISAUtils* sautils;

// MenuSZK
#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

#define menuDebug menuSZK->debug

#define NO_PED_FOUND -1
#define CHASE_MAX_VEHICLE_SPEED 40.0f
#define CHASE_MAX_POLICE_SPEED 40.0f

#include "hooks.h"

#include <functional>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>

#include "EventListener.h"

inline bool HasCleo() { return cleo != nullptr; }

inline bool blockInput = false;

inline void* textureBlip = nullptr;
inline void* textureCircle = nullptr;
inline void* texturePoliceDP = nullptr;

inline CVector* g_playerPosition = new CVector(0, 0, 0);
inline CVector2D g_defaultMenuPosition = CVector2D(400, 200);

inline EventListener<int>* g_onPedLeaveVehicle = new EventListener<int>();
inline EventListener<int>* g_onVehicleDestroy = new EventListener<int>();

#define COLOR_CRIMINAL CRGBA(255, 0, 0)
#define COLOR_YELLOW CRGBA(255, 255, 0)
#define COLOR_POLICE CRGBA(0, 150, 255)

template <typename T>
inline T* loadInterface(T** out, std::string name, bool optional = false)
{
    fileLog->Log("Looking for " + name + "...");

    void* interface = GetInterface(name.c_str());

    *out = static_cast<T*>(interface);

    if (*out)
    {
        fileLog->Log(name + " loaded");
    } else {

        if(optional)
        {
            fileLog->Log(name + " was not found, but its optional");
        } else {
            
            fileLog->Error(name + " was not found");
        }
    }

    return *out;
}

inline double distanceBetweenPoints(CVector point1, CVector point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;

    return sqrt( dx * dx + dy * dy + dz * dz );
}

inline double distanceBetweenPoints2D(CVector2D point1, CVector2D point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;

    return sqrt(dx * dx + dy * dy);
}

inline std::string VectorToString(const CVector& vec, int precision = 2)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision);
    oss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return oss.str();
}

inline int getRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

inline bool calculateProbability(float chance)
{
    int i = getRandomNumber(0, 99);
    return i < (int)(chance * 100.0f);
}
