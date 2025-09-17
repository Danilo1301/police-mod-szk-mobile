#include "Audios.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

IAudio* Audios::audioEncostaCarro = NULL;
IAudio* Audios::audioDesceMaoCabeca = NULL;
IAudio* Audios::audioLiberado = NULL;

void Audios::CreateAudios()
{
    audioEncostaCarro = menuSZK->CreateAudio(getPathFromAssets("audios/officer/pull_over_car.wav"));
    audioDesceMaoCabeca = menuSZK->CreateAudio(getPathFromAssets("audios/officer/exit_vehicle_hands_up.wav"));
    audioLiberado = menuSZK->CreateAudio(getPathFromAssets("audios/officer/all_clear.wav"));
}