#include "AICop.h"

#include "CleoFunctions.h"

AICop::~AICop()
{

}

void AICop::Start()
{
    fileLog->Log("AICop: Start");
    menuDebug->AddLine("AICop started");
} 

void AICop::Update()
{
    if(!ACTOR_DEFINED(pedRef)) return;
}