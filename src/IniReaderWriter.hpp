#pragma once

#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>

// se CVector não estiver incluso aqui, incluir seu header antes
// #include "CVector.h"

class IniReaderWriter
{
public:
    // ordem preservada dentro de cada seção
    using Section = std::vector<std::pair<std::string, std::string>>;
    using Data = std::unordered_map<std::string, Section>;

    // -------------------------------------------------------------
    // Load / Save
    // -------------------------------------------------------------

    bool LoadFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::stringstream ss;
        ss << file.rdbuf();
        return LoadFromString(ss.str());
    }

    bool SaveToFile(const std::string& path)
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        for (const auto& sectionPair : data)
        {
            file << "[" << sectionPair.first << "]\n";

            for (const auto& kv : sectionPair.second)
                file << kv.first << " = " << kv.second << "\n";

            file << "\n";
        }
        return true;
    }

    bool LoadFromString(const std::string& content)
    {
        data.clear();
        std::istringstream iss(content);
        std::string line, currentSection;

        while (std::getline(iss, line))
        {
            Trim(line);

            if (line.empty()) continue;
            if (line[0] == '#' || line[0] == ';') continue;

            if (line.front() == '[' && line.back() == ']')
            {
                currentSection = line.substr(1, line.size() - 2);
                Trim(currentSection);
            }
            else
            {
                auto eq = line.find('=');
                if (eq == std::string::npos)
                    continue;

                std::string key = line.substr(0, eq);
                std::string value = line.substr(eq + 1);

                Trim(key);
                Trim(value);

                Set(currentSection, key, value); // usa Set() para manter ordem
            }
        }
        return true;
    }

    // -------------------------------------------------------------
    // Getters
    // -------------------------------------------------------------

    std::string Get(const std::string& section, const std::string& key, const std::string& def = "") const
    {
        auto sit = data.find(section);
        if (sit == data.end())
            return def;

        // procurar dentro do vector
        for (const auto& kv : sit->second)
        {
            if (kv.first == key)
                return kv.second;
        }

        return def;
    }

    int GetInt(const std::string& section, const std::string& key, int def = 0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        try { return std::stoi(value); }
        catch (...) { return def; }
    }

    double GetDouble(const std::string& section, const std::string& key, double def = 0.0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        try { return std::stod(value); }
        catch (...) { return def; }
    }

    CVector GetCVector(const std::string& section, const std::string& key, CVector def) const
    {
        float x = GetDouble(section, key + "_x", def.x);
        float y = GetDouble(section, key + "_y", def.y);
        float z = GetDouble(section, key + "_z", def.z);

        return CVector(x, y, z);
    }

    bool GetBool(const std::string& section, const std::string& key, bool def = false) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        return (value == "1" || value == "true");
    }

    // -------------------------------------------------------------
    // Setters (preservam a ordem)
    // -------------------------------------------------------------

    void Set(const std::string& section, const std::string& key, const std::string& value)
    {
        auto& list = data[section];

        // atualizar se já existir
        for (auto& kv : list)
        {
            if (kv.first == key)
            {
                kv.second = value;
                return;
            }
        }

        // adicionar no fim (mantém ordem)
        list.emplace_back(key, value);
    }

    void SetInt(const std::string& section, const std::string& key, int value)
    {
        Set(section, key, std::to_string(value));
    }

    void SetDouble(const std::string& section, const std::string& key, double value)
    {
        Set(section, key, std::to_string(value));
    }

    void SetBool(const std::string& section, const std::string& key, bool value)
    {
        Set(section, key, value ? "true" : "false");
    }

    void SetCVector(const std::string& section, const std::string& key, const CVector& value)
    {
        SetDouble(section, key + "_x", value.x);
        SetDouble(section, key + "_y", value.y);
        SetDouble(section, key + "_z", value.z);
    }

    Data* GetData()
    {
        return &data;
    }

private:

    // trim
    static void Trim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    Data data;
};
