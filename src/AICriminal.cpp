#include "AICriminal.h"

#include "CleoFunctions.h"

AICriminal::~AICriminal()
{

}

void AICriminal::Start()
{
    fileLog->Log("AICriminal: Start");
} 

void AICriminal::Update()
{
    if(!ACTOR_DEFINED(pedRef)) return;
}