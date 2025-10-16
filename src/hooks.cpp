#include "hooks.h"

void DoHooks()
{
    [[maybe_unused]] void* hGTASA = dlopen("libGTASA.so", RTLD_LAZY);
    [[maybe_unused]] uintptr_t pGTASA = aml->GetLib("libGTASA.so");

    
}   