#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include "rw/rwcore.h"

#include "simpleGta.h"
#include "utils.h"

#include "Log.h"
#include "PoliceMod.h"

// CLEO 2.0.1.3
#include "cleo.h"
cleo_ifs_t* cleo = NULL;

// SAUtils 1.6
#include "isautils.h"
ISAUtils* sautils = NULL;

// MenuSZK
#include "menuSZK/IMenuSZK.h"
IMenuSZK* menuSZK = NULL;

MYMODCFG(net.daniloszk.policemod, Police Mod SZK, 0.1.0, DaniloSZK)

// --------------------------------------

extern "C" void OnModPreLoad()
{
    char modFolder[512];
    sprintf(modFolder, "%s/policeMod", aml->GetConfigPath());
    
    createDirectoryIfNotExists(modFolder);

    Log::Init(modFolder);
    Log::file->clearLog();
    Log::internal->clearLog();

    logToFile("Log initialized");
    logInternal("Internal log initialized");

    logToFile("Mod preload...");
}

extern "C" void OnModLoad()
{
    logToFile("Mod load...");

    loadInterface(&cleo, "CLEO");
    if(!cleo) return;

    loadInterface(&menuSZK, "MenuSZK");
    if(!menuSZK) return;

    loadInterface(&sautils, "SAUtils");
    if(!sautils) return;

    logToFile("Finding addresses...");

    void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    uintptr_t gameAddr = (uintptr_t)cleo->GetMainLibraryLoadAddress();

    menuSZK->onFirstUpdate->Add([]() {
        policeMod->Initialize();
    });
}
