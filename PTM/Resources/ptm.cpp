//=============================================================================
//	INCLUDES
//=============================================================================
#include <windows.h>
#include <SDL.h>
#include <map>
#include <string>
#include <vector>

//=============================================================================
//	FORWARD DECLARATIONS
//=============================================================================
class Ptm;
class System;
class Screen;

//=============================================================================
//	SYSTEM
//=============================================================================
class System {
public:
	System() {}
	~System() {}

	void Exit() {}
	void Halt() {}
	void ProcessGlobalEvents() {}

	void ShowMsgBox(std::string msg) {
		MessageBox(NULL, msg.c_str(), "", MB_ICONINFORMATION | MB_OK);
	}
};

//=============================================================================
//	SCREEN
//=============================================================================
class Screen {
public:
	Screen() {}
	~Screen() {}

	void OpenWindow(int w, int h, int z, bool full) {}
	void SetBackColorIx(int ix) {}
	void Clear() {}
};

//=============================================================================
//	PTM
//=============================================================================
class Ptm {
public:
	System* system = nullptr;
	Screen* screen = nullptr;

	Ptm() {}
	~Ptm() {
		delete system;
		delete screen;
	}

// _PTM_BEGIN_CLASS_
};

//=============================================================================
//	MAIN
//=============================================================================
int main(int argc, char* argv[]) {
	Ptm* ptm = new Ptm();
	ptm->__ptm_main__();
	delete ptm;
	return 0;
}
