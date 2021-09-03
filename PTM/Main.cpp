#include <TileGameLib.h>
#include "Machine.h"
using namespace TileGameLib;

#define AUTORUN_FILE "autorun.ptm"

int main(int argc, char** args)
{
	std::string programFile = "main.prg";
	if (TFile::Exists(AUTORUN_FILE)) {
		auto autorun = TFile::ReadLines(AUTORUN_FILE);
		if (autorun.size() > 0) {
			programFile = TString::Trim(autorun[0]);
		}
	}

	Machine* machine = new Machine();
	machine->LoadProgram(programFile);
	machine->Run();
	delete machine;

	return 0;
}
