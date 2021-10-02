//=============================================================================
//	INCLUDES
//=============================================================================
#include <windows.h>
#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
//=============================================================================
//	CONSTANTS
//=============================================================================
#define LOG_FILE_NAME "ptm.log"
//=============================================================================
//	MACROS
//=============================================================================
#define VARGS(x) char x[256] = { 0 }; va_list arg; va_start(arg, fmt); vsprintf(str, fmt, arg); va_end(arg);
//=============================================================================
//	TYPEDEFS
//=============================================================================
typedef int PaletteIx;
typedef int TilesetIx;
typedef int LayerIx;
typedef int ColorRgb;
typedef std::string ColorCodes;
//=============================================================================
//	ENUMERATIONS
//=============================================================================

//=============================================================================
//	DECLARATIONS
//=============================================================================
namespace String {
	std::string Format(const char* fmt, ...);
	std::string ToUpper(std::string text);
	bool ToBool(std::string str);
	std::string Repeat(char ch, int times);
}
namespace Debug {
	std::ofstream File;
	void DeleteFile();
	void ClearFile();
	void Log(const char* fmt, ...);
	void MsgBox(const char* fmt, ...);
}
namespace System {
	SDL_Event SDLGlobalEvent = { 0 };
	int PauseTime = 0;

	void Init();
	void Exit();
	void Abort(std::string msg);
	void Halt();
	void Eof();
	void Pause(int time);
	void ProcessGlobalEvents();
}
namespace ColorCode {
	const char Transparent = '0';
	const char C1 = '1';
	const char C2 = '2';
	const char C3 = '3';
	const char C4 = '4';
};
struct TilePixelData {
	static const int TileWidth = 8;
	static const int TileHeight = 8;
	static const int Length = TileWidth * TileHeight;
	ColorCodes Pixels = String::Repeat(ColorCode::Transparent, Length);
};
struct ScreenLayer {
	PaletteIx* Pixels = nullptr;
	int Length = 0;
	int Width = 0;
	int Height = 0;
	bool Enabled = false;
};
namespace Screen {
	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
	SDL_Texture* MainTexture = nullptr;
	int Width = 0;
	int Height = 0;
	int Zoom = 0;
	int Cols = 0;
	int Rows = 0;
	int WndWidth = 0;
	int WndHeight = 0;
	ColorRgb* RgbBuffer = nullptr;
	int RgbBufferLen = 0;
	std::vector<ColorRgb> Palette;
	std::vector<TilePixelData> Tileset;
	const int DefaultPaletteSize = 256;
	const int DefaultTilesetSize = 256;
	PaletteIx BackColor = 0;
	const int LayerCount = 3;
	ScreenLayer Layers[LayerCount];
	
	void OpenWindow(int w, int h, int z, int full);
	ScreenLayer CreateLayer(int w, int h);
	void DestroyLayer(LayerIx ix);
	void CloseWindow();
	void InitDefaultTileset();
	void ClearTileset();
	void AddBlankTilesetTile();
	void SetTilesetTile(TilesetIx ix, ColorCodes pixels);
	void SetTilesetTile(TilesetIx ix, int row, ColorCodes pixels);
	void SetTilesetTile(TilesetIx ix, 
		ColorCodes row0, ColorCodes row1, ColorCodes row2, ColorCodes row3, 
		ColorCodes row4, ColorCodes row5, ColorCodes row6, ColorCodes row7);
	ColorCodes& GetTilesetTile(TilesetIx ix);
	void AssertTilesetIxRange(TilesetIx ix);
	void InitDefaultPalette();
	void ClearPalette();
	void AddPaletteColor(ColorRgb rgb);
	void SetPaletteColor(PaletteIx ix, ColorRgb rgb);
	ColorRgb GetPaletteColor(PaletteIx ix);
	void AssertPaletteIxRange(PaletteIx ix);
	void SetBackColor(PaletteIx ix);
	void ClearBackground();
	void AssertLayerIxRange(LayerIx ix);
	void EnableLayer(LayerIx ix, bool enable);
	void ClearLayers();
	void ClearLayer(LayerIx ix);
	void FillLayer(LayerIx layer, PaletteIx color);
	void ToggleFull();
	void DrawLayers();
	void DrawLayer(LayerIx ix);
	void Update();
	void PutPixel(LayerIx layerIx, int x, int y, PaletteIx color);
	void PutTile(TilesetIx tileIx, LayerIx layerIx, int x, int y,
		PaletteIx c1, PaletteIx c2, PaletteIx c3, PaletteIx c4);
}

// _BEGIN_DECLS_

//=============================================================================
//	DEFINITIONS
//=============================================================================
std::string String::Format(const char* fmt, ...) {
	VARGS(str);
	return str;
}
std::string String::ToUpper(std::string text) {
	for (unsigned i = 0; i < text.size(); i++)
		text[i] = toupper(text[i]);

	return text;
}
bool String::ToBool(std::string str) {
	str = ToUpper(str);
	return str == "YES" || str == "ON" || str == "TRUE" || std::atoi(str.c_str()) > 0;
}
std::string String::Repeat(char ch, int times)
{
	std::string str = "";
	for (int i = 0; i < times; i++) {
		str.push_back(ch);
	}
	return str;
}
void Debug::DeleteFile() {
	remove(LOG_FILE_NAME);
}
void Debug::ClearFile() {
	File.open(LOG_FILE_NAME, std::ios::out | std::ios::trunc);
	File.flush();
	File.close();
}
void Debug::Log(const char* fmt, ...) {
	VARGS(str);
	File.open(LOG_FILE_NAME, std::ios::out | std::ios::app | std::ios::binary);
	File << str << std::endl;
	File.flush();
	File.close();
}
void Debug::MsgBox(const char* fmt, ...) {
	VARGS(str);
	MessageBox(NULL, str, "", MB_ICONINFORMATION | MB_OK);
}
void System::Init() {
	Debug::ClearFile();
	Debug::Log("System started");
}
void System::Exit() {
	Screen::CloseWindow();
	Debug::Log("System exited");
	exit(0);
}
void System::Abort(std::string msg) {
	Debug::Log(msg.c_str());
	MessageBox(NULL, msg.c_str(), "", MB_ICONERROR | MB_OK);
	Exit();
}
void System::Halt() {
	while (true) {
		ProcessGlobalEvents();
		SDL_Delay(1);
	}
}
void System::Eof() {
	Abort("Program ended without EXIT");
}
void System::ProcessGlobalEvents() {
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
void System::Pause(int time) {
	PauseTime = time;
	while (PauseTime > 0) {
		ProcessGlobalEvents();
		SDL_Delay(1);
		PauseTime--;
	}
}
ScreenLayer Screen::CreateLayer(int w, int h) {
	ScreenLayer layer;
	layer.Width = w;
	layer.Height = h;
	layer.Length = w * h;
	layer.Pixels = new int[layer.Length];
	for (int i = 0; i < layer.Length; i++)
		layer.Pixels[i] = 0;

	return layer;
}
void Screen::DestroyLayer(LayerIx ix) {
	ScreenLayer& layer = Layers[ix];
	delete[] layer.Pixels;
	layer.Pixels = nullptr;
	layer.Length = 0;
}
void Screen::OpenWindow(int w, int h, int z, int full) {
	Width = w;
	Height = h;
	Zoom = z;
	WndWidth = w * z;
	WndHeight = h * z;
	Cols = w / TilePixelData::TileWidth;
	Rows = h / TilePixelData::TileHeight;
	RgbBufferLen = sizeof(int) * w * h;
	RgbBuffer = new int[RgbBufferLen];

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	Window = SDL_CreateWindow("",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WndWidth, WndHeight, full ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

	Renderer = SDL_CreateRenderer(Window, -1,
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	
	SDL_RenderSetLogicalSize(Renderer, Width, Height);
	MainTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);

	for (int i = 0; i < LayerCount; i++)
		Layers[i] = CreateLayer(w, h);

	InitDefaultTileset();
	InitDefaultPalette();
	SetBackColor(0);
	ClearBackground();
	Update();

	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_RaiseWindow(Window);
}
void Screen::CloseWindow() {
	if (Window == nullptr)
		return;

	for (int i = 0; i < LayerCount; i++)
		DestroyLayer(i);
	
	SDL_DestroyTexture(MainTexture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	delete[] RgbBuffer;
	RgbBuffer = nullptr;
	RgbBufferLen = 0;
}
void Screen::InitDefaultTileset() {
	ClearTileset();
	for (int i = 0; i < DefaultTilesetSize; i++)
		AddBlankTilesetTile();
}
void Screen::ClearTileset() {
	Tileset.clear();
}
void Screen::AddBlankTilesetTile() {
	TilePixelData tile;
	Tileset.push_back(tile);
}
void Screen::SetTilesetTile(TilesetIx ix, ColorCodes pixels) {
	AssertTilesetIxRange(ix);
	Tileset[ix].Pixels = pixels;
}
void Screen::SetTilesetTile(TilesetIx ix, int row, ColorCodes pixels) {
	AssertTilesetIxRange(ix);
	int pixelIx = row * TilePixelData::TileWidth;
	for (int i = 0; i < TilePixelData::TileWidth; i++)
		Tileset[ix].Pixels[pixelIx + i] = pixels[i];
}
void Screen::SetTilesetTile(TilesetIx ix,
	ColorCodes row0, ColorCodes row1, ColorCodes row2, ColorCodes row3,
	ColorCodes row4, ColorCodes row5, ColorCodes row6, ColorCodes row7) {
	
	AssertTilesetIxRange(ix);
	std::stringstream codes;
	codes << row0;
	codes << row1;
	codes << row2;
	codes << row3;
	codes << row4;
	codes << row5;
	codes << row6;
	codes << row7;
	SetTilesetTile(ix, codes.str());
}
ColorCodes& Screen::GetTilesetTile(TilesetIx ix) {
	AssertTilesetIxRange(ix);
	return Tileset[ix].Pixels;
}
void Screen::AssertTilesetIxRange(TilesetIx ix) {
	if (ix < 0 || ix >= Tileset.size())
		System::Abort(String::Format("Tileset index out of range: %i", ix));
}
void Screen::InitDefaultPalette() {
	ClearPalette();
	for (int i = 0; i < DefaultPaletteSize; i++)
		AddPaletteColor(0x000000);
}
void Screen::ClearPalette() {
	Palette.clear();
}
void Screen::AddPaletteColor(ColorRgb rgb) {
	Palette.push_back(rgb);
}
void Screen::SetPaletteColor(PaletteIx ix, ColorRgb rgb) {
	AssertPaletteIxRange(ix);
	Palette[ix] = rgb;
}
ColorRgb Screen::GetPaletteColor(PaletteIx ix) {
	AssertPaletteIxRange(ix);
	return Palette[ix];
}
void Screen::AssertPaletteIxRange(PaletteIx ix) {
	if (ix < 0 || ix >= Palette.size())
		System::Abort(String::Format("Palette index out of range: %i", ix));
}
void Screen::SetBackColor(PaletteIx ix) {
	BackColor = ix;
}
void Screen::ClearBackground() {
	for (int i = 0; i < RgbBufferLen; i++)
		RgbBuffer[i] = GetPaletteColor(BackColor);
}
void Screen::AssertLayerIxRange(LayerIx ix) {
	if (ix < 0 || ix >= LayerCount)
		System::Abort(String::Format("Layer index out of range: %i", ix));
}
void Screen::EnableLayer(LayerIx ix, bool enable) {
	AssertLayerIxRange(ix);
	Layers[ix].Enabled = enable;
}
void Screen::ClearLayers() {
	for (int i = 0; i < LayerCount; i++)
		ClearLayer(i);
}
void Screen::ClearLayer(LayerIx ix) {
	AssertLayerIxRange(ix);
	FillLayer(ix, 0);
}
void Screen::FillLayer(LayerIx ix, PaletteIx color) {
	AssertLayerIxRange(ix);
	ScreenLayer& layer = Layers[ix];
	for (int i = 0; i < layer.Length; i++)
		layer.Pixels[i] = color;
}
void Screen::ToggleFull() {
	Uint32 fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	Uint32 isFullscreen = SDL_GetWindowFlags(Window) & fullscreenFlag;
	SDL_SetWindowFullscreen(Window, isFullscreen ? 0 : fullscreenFlag);
	SDL_ShowCursor(isFullscreen);
	Update();
}
void Screen::DrawLayers() {
	ClearBackground();
	for (int i = 0; i < LayerCount; i++)
		DrawLayer(i);
}
void Screen::DrawLayer(LayerIx layerIx) {
	AssertLayerIxRange(layerIx);
	ScreenLayer& layer = Layers[layerIx];
	for (int i = 0; i < layer.Length; i++) {
		if (layer.Enabled) {
			if (layer.Pixels[i] > 0) {
				RgbBuffer[i] = GetPaletteColor(layer.Pixels[i]);
			}
			else if (layerIx == 0) {
				RgbBuffer[i] = GetPaletteColor(BackColor);
			}
		}
	}
}
void Screen::Update() {
	static int pitch;
	static void* pixels;
	SDL_LockTexture(MainTexture, nullptr, &pixels, &pitch);
	SDL_memcpy(pixels, RgbBuffer, RgbBufferLen);
	SDL_UnlockTexture(MainTexture);
	SDL_RenderCopy(Renderer, MainTexture, nullptr, nullptr);
	SDL_RenderPresent(Renderer);
}
void Screen::PutPixel(LayerIx layerIx, int x, int y, PaletteIx color) {
	AssertLayerIxRange(layerIx);
	if (x >= 0 && y >= 0 && x < Layers[layerIx].Width && y < Layers[layerIx].Height)
		Layers[layerIx].Pixels[y * Layers[layerIx].Width + x] = color;
}
void Screen::PutTile(TilesetIx tileIx, LayerIx layerIx, int x, int y,
	PaletteIx c1, PaletteIx c2, PaletteIx c3, PaletteIx c4) {
	
	AssertLayerIxRange(layerIx);

	int initialX = x;
	int widthCounter = 0;
	ColorCodes& tile = GetTilesetTile(tileIx);

	for (char pix : tile) {
		PaletteIx color = -1;
		if (pix == ColorCode::Transparent)
			color = 0;
		else if (pix == ColorCode::C1)
			color = c1;
		else if (pix == ColorCode::C2)
			color = c2;
		else if (pix == ColorCode::C3)
			color = c3;
		else if (pix == ColorCode::C4)
			color = c4;
		else {
			System::Abort(String::Format("Invalid color code: %c", pix));
			return;
		}

		PutPixel(layerIx, x, y, color);
		x++;
		widthCounter++;

		if (widthCounter >= TilePixelData::TileWidth) {
			widthCounter = 0;
			x = initialX;
			y++;
		}
	}
}

// _BEGIN_DEFS_
