#include "DocumentWindow.h"

#include "utils.h"

#include "menuSZK/IMenuSZK.h"
extern IMenuSZK* menuSZK;

DocumentWindow::DocumentWindow(std::string title, Ped* ped)
{
    this->ped = ped;
    window = menuSZK->CreateWindow(400, 30, 900, title);
}

void DocumentWindow::MakeWindow()
{
    // dont make window here
}