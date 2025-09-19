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
#include <cstdlib>
#include <ctime>

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

static std::string randomPlate()
{
    // Letras permitidas (pode adicionar/remover)
    static const std::vector<std::string> prefixes = {
        "EGC", "BRA", "MNT", "CPU", "LVR", "SJP", "FGT", "QXZ", "HJK", "VPE", 
        "NDO", "RSC", "YAB", "ZGM", "PQL", "TJS", "WFM", "LXN", "CBF", "GVN"
    };

    // RNG (random device + Mersenne Twister)
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Sorteia prefixo
    std::uniform_int_distribution<> prefixDist(0, prefixes.size() - 1);
    std::string plate = prefixes[prefixDist(gen)];

    // Sorteia números (0000–9999)
    std::uniform_int_distribution<> numDist(0, 9999);
    int number = numDist(gen);

    // Formata para sempre ter 4 dígitos
    char buf[10];
    snprintf(buf, sizeof(buf), " %04d", number);

    plate += buf;
    return plate;
}

static std::string randomPlateLimited()
{
    // Letras permitidas para o prefixo
    static const std::vector<char> letters = {'A', 'C', 'E', 'P'};
    // Números permitidos para os dígitos
    static const std::vector<char> numbers = {'3', '4', '6', '7', '8'};

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> distLetters(0, letters.size() - 1);
    std::uniform_int_distribution<> distNumbers(0, numbers.size() - 1);

    std::string plate;

    // 3 letras
    for (int i = 0; i < 3; ++i)
        plate += letters[distLetters(gen)];

    plate += " "; // espaço antes dos números

    // 4 números
    for (int i = 0; i < 4; ++i)
        plate += numbers[distNumbers(gen)];

    return plate;
}

// Converte string para minúsculas
static std::string to_lower(const std::string& str)
{
    std::string out = str;
    std::transform(out.begin(), out.end(), out.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return out;
}

static int getRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

static bool calculateProbability(float chance)
{
    int i = getRandomNumber(0, 99);
    return i < (int)(chance * 100.0f);
}

static std::string gerarCatHab()
{
    std::string cat;

    // 50% chance de ter A
    if (calculateProbability(0.50f))
        cat += "A";

    // 90% chance de ter B
    if (calculateProbability(0.90f))
    {
        cat += "B";

        // 8% chance de ter C
        if (calculateProbability(0.08f))
            cat += "C";

        // 3% chance de ter D
        if (calculateProbability(0.03f))
            cat += "D";

        // 3% chance de ter E
        if (calculateProbability(0.03f))
        {
            cat += "E";
        }
    }
        
    if (cat.empty())
        cat = "B"; // fallback, sempre tem pelo menos B

    return cat;
}

// Pega ano atual
static int getCurrentYear()
{
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    return now->tm_year + 1900;
}

// Retorna string no formato DD/MM/AAAA
static std::string gerarValidadeCNH(int minYear, int maxYear)
{
    int ano = getRandomNumber(minYear, maxYear);
    int mes = getRandomNumber(1, 12);

    int diaMax;
    switch (mes) {
        case 2: // Fevereiro (ignora bissexto por simplicidade)
            diaMax = 28;
            break;
        case 4: case 6: case 9: case 11: // Abril, Junho, Setembro, Novembro
            diaMax = 30;
            break;
        default:
            diaMax = 31;
    }

    int dia = getRandomNumber(1, diaMax);

    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d", dia, mes, ano);

    return std::string(buffer);
}

static char randomCharVIN()
{
    static const char chars[] =
        "ABCDEFGHJKLMNPRSTUVWXYZ"  // letras válidas (sem I, O, Q)
        "0123456789";
    int idx = rand() % (sizeof(chars) - 1);
    return chars[idx];
}

static std::string gerarChassi()
{
    std::string vin;

    // WMI fixo brasileiro (ex.: 9BW = VW Brasil, mas pode variar)
    vin += "9BW";

    // VDS (4–9): aleatório
    for (int i = 0; i < 6; i++)
        vin += randomCharVIN();

    // VIS (10–17): aleatório
    for (int i = 0; i < 8; i++)
        vin += randomCharVIN();

    return vin;
}

static std::string gerarRenavam()
{
    std::string renavam;
    renavam.reserve(11);

    for (int i = 0; i < 11; ++i)
    {
        int digit = getRandomNumber(0, 9);
        renavam.push_back('0' + digit);
    }

    return renavam;
}