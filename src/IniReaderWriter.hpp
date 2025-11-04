#pragma once

#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>

class IniReaderWriter
{
public:
    // estrutura: categoria -> (chave -> valor)
    using Section = std::unordered_map<std::string, std::string>;
    using Data = std::unordered_map<std::string, Section>;

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

                data[currentSection][key] = value;
            }
        }
        return true;
    }

    std::string Get(const std::string& section, const std::string& key, const std::string& def = "") const
    {
        auto sit = data.find(section);
        if (sit == data.end()) return def;
        auto kit = sit->second.find(key);
        if (kit == sit->second.end()) return def;
        return kit->second;
    }

    int GetInt(const std::string& section, const std::string& key, int def = 0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return def;
        }
    }

    double GetDouble(const std::string& section, const std::string& key, double def = 0.0) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        try
        {
            return std::stod(value);
        }
        catch (...)
        {
            return def;
        }
    }

    bool GetBool(const std::string& section, const std::string& key, bool def = false) const
    {
        std::string value = Get(section, key, "");
        if (value.empty()) return def;

        // deixa tudo minúsculo
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        if (value == "1" || value == "true")
            return true;

        return false;
    }

    void Set(const std::string& section, const std::string& key, const std::string& value)
    {
        data[section][key] = value;
    }

    void SetInt(const std::string& section, const std::string& key, int value)
    {
        data[section][key] = std::to_string(value);
    }

    void SetBool(const std::string& section, const std::string& key, bool value)
    {
        data[section][key] = value ? "true" : "false";
    }

    Data* GetData()
    {
        return &data;
    }

private:
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