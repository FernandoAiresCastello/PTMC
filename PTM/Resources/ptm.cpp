//#############################################################################
//#     INCLUDES                                                              #
//#############################################################################
#include <windows.h>
#include <SDL.h>
#include <string>
#include <vector>
//#############################################################################
//#     DEFINES                                                               #
//#############################################################################
#define WINDOW_TITLE "PTM"
//#############################################################################
//#     TYPEDEFS                                                              #
//#############################################################################
typedef int ColorRGB;
typedef int PaletteIx;
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
SDL_Event SDLGlobalEvent = { 0 };
int PauseTime = 0;
PaletteIx ScrBackColorIx = 0;
std::vector<ColorRGB> ScrPalette;
//#############################################################################
//#     FUNCTION PROTOTYPES                                                   #
//#############################################################################
void SysExit();
void SysHalt();
void SysPause(int ms);
void SysProcGlobalEvents();
void DbgMsgBox(std::string value, std::string title = "");
void DbgMsgBox(int value, std::string title = "");
void SysAbort(std::string msg);
void ScrOpenWindow(int scrW, int scrH, int scrZ, bool full);
void ScrCloseWindow();
void ScrUpdate();
void ScrClearToRGB(ColorRGB rgb);
void ScrClearToBackColor(PaletteIx ix);
void ScrToggleFull();
void ScrSetPixel(int x, int y, ColorRGB rgb);
void ScrSetBackColorIx(PaletteIx ix);
void ScrInitDefaultPalette();
void ScrPaletteAdd(ColorRGB rgb);
ColorRGB& ScrPaletteGet(PaletteIx ix);
void AssertPaletteIx(PaletteIx ix);
//#############################################################################
//#     FUNCTIONS                                                             #
//#############################################################################
void AssertPaletteIx(PaletteIx ix)
{
	if (ix < 0 || ix >= ScrPalette.size())
		SysAbort("Palette index out of range");
}
//#############################################################################
void ScrPaletteAdd(ColorRGB rgb)
{
	ScrPalette.push_back(rgb);
}
//#############################################################################
ColorRGB& ScrPaletteGet(PaletteIx ix)
{
	AssertPaletteIx(ix);
	return ScrPalette[ix];
}
//#############################################################################
void ScrInitDefaultPalette()
{
	ScrPaletteAdd(0x000000);
	ScrPaletteAdd(0xffffff);
}
//#############################################################################
void ScrSetBackColorIx(PaletteIx ix)
{
	AssertPaletteIx(ix);
	ScrBackColorIx = ix;
}
//#############################################################################
void ScrClearToBackColor()
{
	ScrClearToRGB(ScrPaletteGet(ScrBackColorIx));
}
//#############################################################################
void SysAbort(std::string msg)
{
	MessageBox(NULL, msg.c_str(), "PTM - Application error", MB_ICONERROR | MB_OK);
	SysExit();
}
//#############################################################################
void DbgMsgBox(std::string value, std::string title)
{
	std::string msg = title;
	if (!title.empty())
		msg.append("\n\n");
	msg.append(value);
	
	MessageBox(NULL, msg.c_str(), "Debug", MB_ICONINFORMATION | MB_OK);
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
	while (true) {
		SysProcGlobalEvents();
		SDL_Delay(1);
	}
}
//#############################################################################
void SysPause(int ms)
{
	PauseTime = ms;
	while (PauseTime > 0) {
		SysProcGlobalEvents();
		SDL_Delay(1);
		PauseTime--;
	}
}
//#############################################################################
void SysProcGlobalEvents()
{
	SDL_PollEvent(&SDLGlobalEvent);
	if (SDLGlobalEvent.type == SDL_QUIT) {
		SysExit();
	}
	else if (SDLGlobalEvent.type == SDL_KEYDOWN) {
		if (SDLGlobalEvent.key.keysym.sym == SDLK_RETURN && SDL_GetModState() & KMOD_ALT) {
			ScrToggleFull();
		}
	}
}
//#############################################################################
void ScrOpenWindow(int scrW, int scrH, int scrZ, bool full)
{
	ScrInitDefaultPalette();

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
void ScrClearToRGB(ColorRGB rgb)
{
	for (int y = 0; y < ScrHeight; y++)
		for (int x = 0; x < ScrWidth; x++)
			ScrBuf[y * ScrWidth + x] = rgb;
}
//#############################################################################
void ScrClearToBackColor(PaletteIx ix)
{
	ScrClearToRGB(ScrPalette[ix]);
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
void ScrSetPixel(int x, int y, ColorRGB rgb)
{
	if (x >= 0 && y >= 0 && x < ScrWidth && y < ScrHeight)
		ScrBuf[y * ScrWidth + x] = rgb;
}
//#############################################################################
//#     MAIN                                                                  #
//#############################################################################
int main(int argc, char* argv[])
{
	// _PTM_BEGIN_USER_MAIN_
    return 0;
}