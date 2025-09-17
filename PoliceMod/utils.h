#pragma once

#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include <string>
#include <sys/stat.h>
#include <iostream>
#include <filesystem>
#include <sstream>

#include "simpleGta.h"
#include "Log.h"

#include <random>

template <typename T>
static T* loadInterface(T** out, std::string name)
{
    logToFile("Loading interface: ", name, "...");

    void* interface = GetInterface(name.c_str());

    //Log::Level(LOG_LEVEL::LOG_BOTH) << "Interface: " << interface << std::endl;

    *out = static_cast<T*>(interface);

    if (*out) logToFile(name, " loaded");
    else logToFile(name, " was not found!");

    return *out;
}

static bool createDirectoryIfNotExists(const std::string& path) {
    namespace fs = std::filesystem;

    try {
        if (!fs::exists(path)) {
            // Cria diretório e todas as pastas pai se não existirem
            return fs::create_directories(path);
        }
    } catch (const fs::filesystem_error& e) {
        //std::cerr << "Erro ao criar diretório: " << e.what() << std::endl;
        return false;
    }

    // Já existia
    return true;
}

static std::string getPathFromFiles(const std::string& localPath)
{
    std::ostringstream oss;
    oss << aml->GetAndroidDataPath() << "/" << localPath;
    return oss.str();
}

static double distanceBetweenPoints(CVector point1, CVector point2)
{
    double dx = point1.x - point2.x;
    double dy = point1.y - point2.y;
    double dz = point1.z - point2.z;

    return sqrt( dx * dx + dy * dy + dz * dz );
}

static std::string getPathFromMenuAssets(const std::string localPath)
{
    std::ostringstream oss;
    oss << aml->GetConfigPath() << "/menuSZK/assets/" << localPath;
    return oss.str();
}

static std::string getPathFromAssets(const std::string localPath)
{
    std::ostringstream oss;
    oss << aml->GetConfigPath() << "/policeMod/assets/" << localPath;
    return oss.str();
}

static std::string randomBirthDate(int minYear, int maxYear) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> dayDist(1, 28); // evita complicação de fevereiro
    std::uniform_int_distribution<> monthDist(1, 12);
    std::uniform_int_distribution<> yearDist(minYear, maxYear);

    int day = dayDist(gen);
    int month = monthDist(gen);
    int year = yearDist(gen);

    std::stringstream ss;
    ss << (day < 10 ? "0" : "") << day << "/"
       << (month < 10 ? "0" : "") << month << "/"
       << year;

    return ss.str();
}

static std::string randomRG() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    ss << dist(gen) << dist(gen) << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-" << dist(gen);

    return ss.str();
}

static std::string randomCPF() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);

    std::stringstream ss;
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 3; ++i) ss << dist(gen);
    ss << "-";
    for (int i = 0; i < 2; ++i) ss << dist(gen);

    return ss.str();
}