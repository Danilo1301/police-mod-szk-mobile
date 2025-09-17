#include "DocumentWindow.h"

#include "utils.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

DocumentWindow::DocumentWindow(std::string title)
{
    window = menuSZK->CreateWindow(400, 50, 900, title);
}

void DocumentWindow::MakeWindow(Ped* ped)
{
    // dont make window here
}