#include <SDL2/SDL.h>
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <bitset>

#define WINDOW_TITLE "Hello World!"
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_FULLSCREEN 0
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 96

//=============================================================================
//      UTIL
//=============================================================================
void PrintDebug(std::string value, std::string title = "")
{
    std::string msg = title;
    if (!title.empty())
        msg.append("\n\n");
    msg.append(value);
	
    MessageBoxA(NULL, msg.c_str(), "Debug", MB_ICONINFORMATION | MB_OK);
}

void PrintDebug(int value, std::string title = "")
{
    PrintDebug(std::to_string(value), title);
}

//=============================================================================
//      PTM
//=============================================================================
class Ptm
{
private:

    SDL_Window* Window;
    SDL_Renderer* Renderer;
    SDL_Texture* ScreenTex;

public:

    Ptm()
    {
    }

    ~Ptm()
    {
    }

    void Init()
    {
        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

        Window = SDL_CreateWindow(WINDOW_TITLE,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

        Renderer = SDL_CreateRenderer(Window, -1,
            SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED || SDL_RENDERER_TARGETTEXTURE);

        SDL_RenderSetLogicalSize(Renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

        ScreenTex = SDL_CreateTexture(Renderer,
            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        SDL_RaiseWindow(Window);
    }

    void Quit()
    {
        SDL_DestroyTexture(ScreenTex);
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(Window);
        SDL_Quit();
    }

    void Test()
    {
        int running = 1;
        SDL_Event event;
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
            }
        }
    }
};

//=============================================================================
//      MAIN
//=============================================================================
int main(int argc, char* argv[])
{
	Ptm* ptm = new Ptm;

	// Begin user code
	// _PTM_BEGIN_USER_MAIN_
	// End user code

	delete ptm;
    return 0;
}