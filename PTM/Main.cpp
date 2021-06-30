#include <SDL.h>
#include <TBRLGPT.h>
#include "SharedObjects.h"
#include "SharedScreen.h"
#include "ProgramEditor.h"
#include "CommandShell.h"
using namespace TBRLGPT;

int main(int argc, char** argv)
{
	InitSharedObjects();
	InitScreen();
	InitCommandShell();
	InitProgramEditor();

	RunCommandShell();

	DestroySharedObjects();
	DestroyScreen();
	DestroyCommandShell();
	DestroyProgramEditor();

	return 0;
}
