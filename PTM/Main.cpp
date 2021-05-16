#include <SDL.h>
#include <TBRLGPT.h>
#include "Util.h"
#include "Machine.h"

using namespace TBRLGPT;

#define INIT_FILE "ptm.ini"

int main(int argc, char** argv)
{
	if (!File::Exists(INIT_FILE)) {
		ShowErrorMessageBox("Init file " INIT_FILE " not found.");
		return 1;
	}

	std::string programFile = String::RemoveAll(File::ReadText(INIT_FILE), "\n\t ");
	if (!File::Exists(programFile)) {
		ShowErrorMessageBox("Program file " + programFile + " not found.");
		return 1;
	}

	Machine* ptm = new Machine();
	ptm->Run(programFile);
	delete ptm;
	return 0;
}
