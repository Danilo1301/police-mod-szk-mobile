#include "ModData.h"

#include <mod/amlmod.h>
#include <sys/stat.h>

ModData::ModData(std::string folderName)
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();

    // Cria pasta principal do mod
    CreateFolder(dataRootPath + "/mods/mods_data/" + folderName + "/");

    // Cria pasta de logs
    CreateFolder(dataRootPath + "/mods/mods_data/logs/");

    this->modFolderPath = dataRootPath + "/mods/mods_data/" + folderName + "/";
}

std::string ModData::GetFileFromAssets(const std::string& localPath)
{
    // Retorna o caminho completo de um arquivo dentro de mods_data/assets/
    return modFolderPath + "/assets/" + localPath;
}

std::string ModData::GetFile(const std::string& localPath)
{
    // Retorna o caminho completo de um arquivo dentro da pasta do mod
    return modFolderPath + "/" + localPath;
}


std::string ModData::GetFileFromMenuSZK(const std::string& localPath)
{
    std::string dataRootPath = aml->GetAndroidDataRootPath();
    std::string menuSZKPath = dataRootPath + "/mods/mods_data/" + "menuSZK" + "/";

    return menuSZKPath + localPath;
}

void ModData::CreateFolder(const std::string& path)
{
    // Cria pasta se não existir
    mkdir(path.c_str(), 0777);
}