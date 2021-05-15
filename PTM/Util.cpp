#include <SDL.h>
#include "Util.h"

void ShowErrorMessageBox(std::string msg)
{
	SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", msg.c_str(), NULL);
}
