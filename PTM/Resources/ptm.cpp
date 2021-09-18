//#############################################################################
//#     INCLUDES                                                              #
//#############################################################################
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
//#############################################################################
//#     DEFINES                                                               #
//#############################################################################
#define WINDOW_TITLE "PTM"
#define WINDOW_FULL 0
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_ZOOM 3
#define WINDOW_WIDTH SCREEN_WIDTH * SCREEN_ZOOM
#define WINDOW_HEIGHT SCREEN_HEIGHT * SCREEN_ZOOM
//#############################################################################
//#     VARIABLES                                                             #
//#############################################################################
const int TileWidth = 8;
const int TileHeight = 8;
SDL_Window* Window = nullptr;
SDL_Renderer* Renderer = nullptr;
SDL_Texture* Scrtx = nullptr;
int* ScrBuf = nullptr;
int ScrPixelFmt = 0;
int ScrBufLength = 0;
int ScrWidth = 0;
int ScrHeight = 0;
int ScrZoom = 0;
int ScrCols = 0;
int ScrRows = 0;
int WndWidth = 0;
int WndHeight = 0;
//#############################################################################
//#     FUNCTION PROTOTYPES                                                   #
//#############################################################################
void SysExit();
void SysHalt();
void DbgMsgBox(std::string value, std::string title = "");
void DbgMsgBox(int value, std::string title = "");
void ScrOpenWindow(int scrW, int scrH, int scrZ, bool full);
void ScrCloseWindow();
void ScrUpdate();
void ScrClearToRGB(int rgb);
void ScrToggleFull();
//#############################################################################
//#     FUNCTIONS                                                             #
//#############################################################################
void DbgMsgBox(std::string value, std::string title)
{
	std::string msg = title;
	if (!title.empty())
		msg.append("\n\n");
	msg.append(value);

	MessageBoxA(NULL, msg.c_str(), "Debug", MB_ICONINFORMATION | MB_OK);
}
//#############################################################################
void DbgMsgBox(int value, std::string title)
{
	DbgMsgBox(std::to_string(value), title);
}
//#############################################################################
void SysExit()
{
	ScrCloseWindow();
	exit(0);
}
//#############################################################################
void SysHalt()
{
	int running = 1;
	SDL_Event e;
	while (running) {
		while (SDL_WaitEvent(&e) >= 0) {
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}
			else if (e.type == SDL_KEYDOWN) {
				if (e.key.keysym.sym == SDLK_RETURN && SDL_GetModState() & KMOD_ALT) {
					ScrToggleFull();
				}
			}
		}
		SDL_Delay(1);
	}
}
//#############################################################################
void ScrOpenWindow(int scrW, int scrH, int scrZ, bool full)
{
	ScrWidth = scrW;
	ScrHeight = scrH;
	ScrZoom = scrZ;
	WndWidth = scrW * scrZ;
	WndHeight = scrH * scrZ;
	ScrCols = scrW / TileWidth;
	ScrRows = scrH / TileHeight;
	ScrPixelFmt = SDL_PIXELFORMAT_ARGB8888;
	ScrBufLength = sizeof(int) * ScrWidth * ScrHeight;
	ScrBuf = new int[ScrBufLength];

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	Window = SDL_CreateWindow(WINDOW_TITLE,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WndWidth, WndHeight, full ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	Renderer = SDL_CreateRenderer(Window, -1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	SDL_RenderSetLogicalSize(Renderer, ScrWidth, ScrHeight);

	Scrtx = SDL_CreateTexture(Renderer,
		ScrPixelFmt, SDL_TEXTUREACCESS_STREAMING, ScrWidth, ScrHeight);

	ScrClearToRGB(0x000000);
	ScrUpdate();

	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_RaiseWindow(Window);
}
//#############################################################################
void ScrCloseWindow()
{
	SDL_DestroyTexture(Scrtx);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	delete[] ScrBuf;
}
//#############################################################################
void ScrUpdate()
{
	static int pitch;
	static void* pixels;

	SDL_LockTexture(Scrtx, nullptr, &pixels, &pitch);
	SDL_memcpy(pixels, ScrBuf, ScrBufLength);
	SDL_UnlockTexture(Scrtx);
	SDL_RenderCopy(Renderer, Scrtx, nullptr, nullptr);
	SDL_RenderPresent(Renderer);
}
//#############################################################################
void ScrClearToRGB(int rgb)
{
	for (int y = 0; y < ScrHeight; y++)
		for (int x = 0; x < ScrWidth; x++)
			ScrBuf[y * ScrWidth + x] = rgb;
}
//#############################################################################
void ScrToggleFull()
{
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 isFullscreen = SDL_GetWindowFlags(Window) & fullscreenFlag;
	SDL_SetWindowFullscreen(Window, isFullscreen ? 0 : fullscreenFlag);
	SDL_ShowCursor(isFullscreen);
	ScrUpdate();
}
//#############################################################################
//#     MAIN                                                                  #
//#############################################################################
int main(int argc, char* argv[])
{
	// _PTM_BEGIN_USER_MAIN_
    return 0;
}