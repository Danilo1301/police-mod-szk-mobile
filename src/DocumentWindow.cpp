#include "DocumentWindow.h"

DocumentWindow::DocumentWindow(std::string title, Ped* ped)
{
    this->ped = ped;
    window = menuSZK->CreateWindow(400, 30, 900, title);
}

DocumentWindow::~DocumentWindow()
{
    
}

void DocumentWindow::MakeWindow()
{
    // dont make window here
}