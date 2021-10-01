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
typedef int ColorRgb;
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
struct ScreenLayer {
	PaletteIx* Pixels = nullptr;
	int Length = 0;
};
namespace Screen {
	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
	SDL_Texture* MainTexture = nullptr;
	const int TileWidth = 8;
	const int TileHeight = 8;
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
	PaletteIx BackColorIx = 0;
	const int LayerCount = 3;
	ScreenLayer Layers[LayerCount];
	
	void OpenWindow(int w, int h, int z, int full);
	ScreenLayer CreateLayer(int w, int h);
	void DestroyLayer(ScreenLayer& layer);
	void CloseWindow();
	void InitDefaultPalette();
	void ClearPalette();
	void AddPaletteColor(ColorRgb rgb);
	void SetBackColorIx(PaletteIx ix);
	ColorRgb GetPaletteColor(PaletteIx ix);
	void ClearBackground();
	void ClearLayers();
	void ClearLayer(ScreenLayer& layer);
	void ToggleFull();
	void DrawLayers();
	void DrawLayer(ScreenLayer& layer, int index);
	void Update();
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
	layer.Length = w * h;
	layer.Pixels = new int[layer.Length];
	ClearLayer(layer);
	return layer;
}
void Screen::DestroyLayer(ScreenLayer& layer) {
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
	Cols = w / TileWidth;
	Rows = h / TileHeight;
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

	InitDefaultPalette();
	SetBackColorIx(0);
	ClearBackground();
	Update();

	SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_RaiseWindow(Window);
}
void Screen::CloseWindow() {
	if (Window == nullptr)
		return;

	for (int i = 0; i < LayerCount; i++)
		DestroyLayer(Layers[i]);
	
	SDL_DestroyTexture(MainTexture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();

	delete[] RgbBuffer;
	RgbBuffer = nullptr;
	RgbBufferLen = 0;
}
void Screen::InitDefaultPalette() {
	ClearPalette();
	AddPaletteColor(0x000000);
	AddPaletteColor(0xffffff);
	AddPaletteColor(0xff0000);
	AddPaletteColor(0x00ff00);
	AddPaletteColor(0x0000ff);
	AddPaletteColor(0xff00ff);
	AddPaletteColor(0xffff00);
	AddPaletteColor(0x00ffff);
	AddPaletteColor(0xff8000);
	AddPaletteColor(0x808080);
}
void Screen::ClearPalette() {
	Palette.clear();
}
void Screen::AddPaletteColor(ColorRgb rgb) {
	Palette.push_back(rgb);
}
void Screen::SetBackColorIx(PaletteIx ix) {
	BackColorIx = ix;
}
ColorRgb Screen::GetPaletteColor(PaletteIx ix) {
	if (ix >= 0 && ix < Palette.size()) {
		return Palette[ix];
	}
	System::Abort(String::Format("Palette index out of range: %i", ix));
	return 0;
}
void Screen::ClearBackground() {
	for (int i = 0; i < RgbBufferLen; i++)
		RgbBuffer[i] = GetPaletteColor(BackColorIx);
}
void Screen::ClearLayers() {
	for (int i = 0; i < LayerCount; i++)
		ClearLayer(Layers[i]);
}
void Screen::ClearLayer(ScreenLayer& layer) {
	for (int i = 0; i < layer.Length; i++)
		layer.Pixels[i] = 0;
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
		DrawLayer(Layers[i], i);
}
void Screen::DrawLayer(ScreenLayer& layer, int index) {
	for (int i = 0; i < layer.Length; i++) {
		PaletteIx& pix = layer.Pixels[i];
		if (i == 0 || pix > 0)
			RgbBuffer[i] = GetPaletteColor(pix);
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

// _BEGIN_DEFS_
