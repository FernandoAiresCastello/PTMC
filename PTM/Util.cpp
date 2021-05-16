#include <SDL.h>
#include "Util.h"

void ShowMessageBox(std::string title, std::string msg)
{
	SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_INFORMATION, title.c_str(), msg.c_str(), NULL);
}

void ShowErrorMessageBox(std::string msg)
{
	SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", msg.c_str(), NULL);
}
