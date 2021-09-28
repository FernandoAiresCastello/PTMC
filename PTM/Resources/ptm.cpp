//#############################################################################
//#     INCLUDES                                                              #
//#############################################################################
#include <windows.h>
#include <SDL.h>
#include <string>
#include <vector>
//#############################################################################
//#     TYPEDEFS                                                              #
//#############################################################################
typedef int ColorRGB;
typedef int PaletteIx;
typedef int TilesetIx;
//#############################################################################
//#     FUNCTION PROTOTYPES                                                   #
//#############################################################################
namespace System {
	SDL_Event SDLGlobalEvent = { 0 };
	int PauseTime = 0;

	void Exit();
	void Halt();
	void Abort(std::string msg);
	void Pause(int ms);
	void ProcGlobalEvents();
}
namespace Debug {
	void MsgBox(std::string value, std::string title = "");
	void MsgBox(int value, std::string title = "");
}

struct TilePixels {
	std::string blankRow = std::string(8, '0');
	std::string Rows[8] = {
		blankRow, blankRow, blankRow, blankRow,
		blankRow, blankRow, blankRow, blankRow
	};

	std::string ToString()
	{
		std::string str = "";
		str.append(Rows[0]);
		str.append(Rows[1]);
		str.append(Rows[2]);
		str.append(Rows[3]);
		str.append(Rows[4]);
		str.append(Rows[5]);
		str.append(Rows[6]);
		str.append(Rows[7]);
		return str;
	}
};

namespace Screen {
	const int TileWidth = 8;
	const int TileHeight = 8;
	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
	SDL_Texture* Texture = nullptr;
	int* Buf = nullptr;
	int PixelFmt = 0;
	int BufLength = 0;
	int Width = 0;
	int Height = 0;
	int Zoom = 0;
	int Cols = 0;
	int Rows = 0;
	int WndWidth = 0;
	int WndHeight = 0;
	PaletteIx BackColorIx = 0;
	std::vector<ColorRGB> Palette;
	std::vector<TilePixels> Tileset;

	void OpenWindow(int scrW, int scrH, int scrZ, bool full);
	void CloseWindow();
	void Update();
	void ClearToRgb(ColorRGB rgb);
	void ClearToPaletteIndex(PaletteIx ix);
	void ClearToBackColor();
	void ToggleFull();
	void SetBackColorIx(PaletteIx ix);
	void InitDefaultPalette();
	void PaletteAdd(ColorRGB rgb);
	void PaletteSet(PaletteIx ix, ColorRGB rgb);
	void InitDefaultTileset();
	void TilesetAdd(TilePixels pixels);
	void TilesetSetPixelRow(TilesetIx ix, int pixelRow, std::string pixels);
	ColorRGB& PaletteGet(PaletteIx ix);
	void AssertPaletteIx(PaletteIx ix);
	void AssertTilesetIx(TilesetIx ix);
	void SetPixelRgb(int x, int y, ColorRGB rgb);
	void SetPixelIndexed(int x, int y, PaletteIx ix);
	void DrawSpriteTile(int x, int y, TilesetIx tileIx, PaletteIx c0, PaletteIx c1, PaletteIx c2, PaletteIx c3);
}
//#############################################################################
//#     FUNCTIONS                                                             #
//#############################################################################
void Screen::AssertTilesetIx(TilesetIx ix)
{
	if (ix < 0 || ix >= Tileset.size())
		System::Abort("Tileset index out of range: " + std::to_string(ix));
}
//#############################################################################
void Screen::AssertPaletteIx(PaletteIx ix)
{
	if (ix < 0 || ix >= Palette.size())
		System::Abort("Palette index out of range: " + std::to_string(ix));
}
//#############################################################################
void Screen::PaletteAdd(ColorRGB rgb)
{
	Palette.push_back(rgb);
}
//#############################################################################
void Screen::PaletteSet(PaletteIx ix, ColorRGB rgb)
{
	AssertPaletteIx(ix);
	Palette[ix] = rgb;
}
//#############################################################################
void Screen::InitDefaultTileset()
{
	for (int i = 0; i < 256; i++)
		TilesetAdd(TilePixels());
}
//#############################################################################
void Screen::TilesetAdd(TilePixels pixels)
{
	Tileset.push_back(pixels);
}
//#############################################################################
void Screen::TilesetSetPixelRow(TilesetIx ix, int pixelRow, std::string pixels)
{
	AssertTilesetIx(ix);
	if (pixelRow < 0 || pixelRow >= TileHeight) {
		System::Abort("Tile row index out of range");
		return;
	}
	if (pixels.empty() || pixels.length() > TileWidth) {
		System::Abort("Invalid tile row pixel data");
		return;
	}
	
	Tileset[ix].Rows[pixelRow] = pixels;
}
//#############################################################################
ColorRGB& Screen::PaletteGet(PaletteIx ix)
{
	AssertPaletteIx(ix);
	return Palette[ix];
}
//#############################################################################
void Screen::InitDefaultPalette()
{
	for (int i = 0; i < 256; i++)
		PaletteAdd(0x000000);

	PaletteSet(0, 0x000000);
	PaletteSet(1, 0xffffff);
}
//#############################################################################
void Screen::SetBackColorIx(PaletteIx ix)
{
	AssertPaletteIx(ix);
	BackColorIx = ix;
}
//#############################################################################
void Screen::ClearToBackColor()
{
	ClearToPaletteIndex(BackColorIx);
}
//#############################################################################
void System::Abort(std::string msg)
{
	MessageBox(NULL, msg.c_str(), "PTM - Application error", MB_ICONERROR | MB_OK);
	Exit();
}
//#############################################################################
void Debug::MsgBox(std::string value, std::string title)
{
	std::string msg = title;
	if (!title.empty())
		msg.append("\n\n");
	msg.append(value);
	
	MessageBox(NULL, msg.c_str(), "Debug", MB_ICONINFORMATION | MB_OK);
}
//#############################################################################
void Debug::MsgBox(int value, std::string title)
{
	Debug::MsgBox(std::to_string(value), title);
}
//#############################################################################
void System::Exit()
{
	Screen::CloseWindow();
	exit(0);
}
//#############################################################################
void System::Halt()
{
	while (true) {
		ProcGlobalEvents();
		SDL_Delay(1);
	}
}
//#############################################################################
void System::Pause(int ms)
{
	PauseTime = ms;
	while (PauseTime > 0) {
		ProcGlobalEvents();
		SDL_Delay(1);
		PauseTime--;
	}
}
//#############################################################################
void System::ProcGlobalEvents()
{
	SDL_PollEvent(&SDLGlobalEvent);
	if (SDLGlobalEvent.type == SDL_QUIT) {
		Exit();
	}
	else if (SDLGlobalEvent.type == SDL_KEYDOWN) {
		if (SDLGlobalEvent.key.keysym.sym == SDLK_RETURN && SDL_GetModState() & KMOD_ALT) {
			Screen::ToggleFull();
		}
	}
}
//#############################################################################
void Screen::OpenWindow(int scrW, int scrH, int scrZ, bool full)
{
	InitDefaultPalette();
	InitDefaultTileset();

	Width = scrW;
	Height = scrH;
	Zoom = scrZ;
	WndWidth = scrW * scrZ;
	WndHeight = scrH * scrZ;
	Cols = scrW / TileWidth;
	Rows = scrH / TileHeight;
	PixelFmt = SDL_PIXELFORMAT_ARGB8888;
	BufLength = sizeof(int) * Width * Height;
	Buf = new int[BufLength];

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	Window = SDL_CreateWindow("",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WndWidth, WndHeight, full ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	Renderer = SDL_CreateRenderer(Window, -1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	SDL_RenderSetLogicalSize(Renderer, Width, Height);

	Texture = SDL_CreateTexture(Renderer, PixelFmt, SDL_TEXTUREACCESS_STREAMING, Width, Height);

	ClearToRgb(0x000000);
	Update();

	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_RaiseWindow(Window);
}
//#############################################################################
void Screen::CloseWindow()
{
	SDL_DestroyTexture(Texture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	delete[] Buf;
}
//#############################################################################
void Screen::Update()
{
	static int pitch;
	static void* pixels;

	SDL_LockTexture(Texture, nullptr, &pixels, &pitch);
	SDL_memcpy(pixels, Buf, BufLength);
	SDL_UnlockTexture(Texture);
	SDL_RenderCopy(Renderer, Texture, nullptr, nullptr);
	SDL_RenderPresent(Renderer);
}
//#############################################################################
void Screen::ClearToRgb(ColorRGB rgb)
{
	for (int y = 0; y < Height; y++)
		for (int x = 0; x < Width; x++)
			Buf[y * Width + x] = rgb;
}
//#############################################################################
void Screen::ClearToPaletteIndex(PaletteIx ix)
{
	for (int y = 0; y < Height; y++)
		for (int x = 0; x < Width; x++)
			Buf[y * Width + x] = Palette[ix];
}
//#############################################################################
void Screen::ToggleFull()
{
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 isFullscreen = SDL_GetWindowFlags(Window) & fullscreenFlag;
	SDL_SetWindowFullscreen(Window, isFullscreen ? 0 : fullscreenFlag);
	SDL_ShowCursor(isFullscreen);
	Update();
}
//#############################################################################
void Screen::SetPixelRgb(int x, int y, ColorRGB rgb)
{
	if (x >= 0 && y >= 0 && x < Width && y < Height)
		Buf[y * Width + x] = rgb;
}
//#############################################################################
void Screen::SetPixelIndexed(int x, int y, PaletteIx ix)
{
	if (x >= 0 && y >= 0 && x < Width && y < Height) {
		AssertPaletteIx(ix);
		Buf[y * Width + x] = Palette[ix];
	}
}
//#############################################################################
void Screen::DrawSpriteTile(int x, int y, TilesetIx tileIx, PaletteIx c0, PaletteIx c1, PaletteIx c2, PaletteIx c3)
{
	AssertTilesetIx(tileIx);

	int px = x;
	TilePixels* tile = &Tileset[tileIx];
	for (char pixelCode : tile->ToString()) {
		PaletteIx ix = -1;
		if (pixelCode == 48)
			ix = c0;
		else if (pixelCode == 49)
			ix = c1;
		else if (pixelCode == 50)
			ix = c2;
		else if (pixelCode == 51)
			ix = c3;
		else {
			System::Abort("Invalid tile pixel code: " + std::to_string((int)pixelCode));
			return;
		}

		SetPixelIndexed(x, y, ix);
		x++;
		if (x >= TileWidth) {
			x = px;
			y++;
		}
	}
}
//#############################################################################
//#     MAIN                                                                  #
//#############################################################################
int main(int argc, char* argv[])
{
	// _PTM_BEGIN_USER_MAIN_
	return 0;
}