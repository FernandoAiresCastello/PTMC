//=============================================================================
//	INCLUDES
//=============================================================================
#include <windows.h>
#include <SDL.h>
#include <map>
#include <string>
#include <vector>
//=============================================================================
//	DECLARATIONS
//=============================================================================
namespace System {
	void ShowMsgBox(std::string msg);
	void Halt();
	void Exit();
	void ProcessGlobalEvents();
}
namespace Screen {
	void OpenWindow(int w, int h, int z, int full);
	void SetBackColorIx(int ix);
	void Clear();
}
// _BEGIN_DECLS_
//=============================================================================
//	DEFINITIONS
//=============================================================================
void System::ShowMsgBox(std::string msg) {
}
void System::Halt() {
}
void System::Exit() {
}
void System::ProcessGlobalEvents() {
}
void Screen::OpenWindow(int w, int h, int z, int full) {
}
void Screen::SetBackColorIx(int ix) {
}
void Screen::Clear() {
}
// _BEGIN_DEFS_