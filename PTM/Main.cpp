#include <SDL.h>
#include <TBRLGPT.h>
#include "SharedObjects.h"
#include "SharedScreen.h"
#include "CommandShell.h"
#include "ProgramEditor.h"
#include "TileMachine.h"
using namespace TBRLGPT;

int main(int argc, char** argv)
{
	InitSharedObjects();
	InitSharedScreen();
	InitCommandShell();
	InitProgramEditor();
	InitTileMachine();

	RunCommandShell();

	DestroySharedObjects();
	DestroySharedScreen();
	DestroyCommandShell();
	DestroyProgramEditor();
	DestroyTileMachine();

	return 0;
}
