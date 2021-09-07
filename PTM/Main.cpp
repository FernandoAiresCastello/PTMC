#include "Command.h"
#include "Machine.h"
#include "Compiler.h"
#include "Program.h"
#include <TileGameLib.h>
using namespace TileGameLib;

int main(int argc, char* argv[])
{
	if (argc < 3) {
		TUtil::Error("Invalid command line. Valid syntax:\n\n"
			"Run:\nptm -r <file>\n\n"
			"Compile:\nptm -c <srcfile> <dstfile>\n\n"
			"Compile then run:\nptm -cr <srcfile> <dstfile>");

		return 1;
	}

	std::string option = argv[1];
	std::string srcpath = argv[2];
	std::string dstpath = argc > 3 ? argv[3] : "";

	if (!TFile::Exists(srcpath)) {
		TUtil::Error(TString::Format("File not found: %s", srcpath.c_str()));
		return 1;
	}

	Command::Init();

	if (option == "-r") {
		Machine* machine = new Machine();
		Program* program = new Program();
		program->LoadBytecode(srcpath);
		machine->Run(program);
		delete program;
		delete machine;
		return 0;
	}

	if (option == "-c") {
		if (dstpath.empty()) {
			TUtil::Error("Missing destination filename");
			return 1;
		}
		Machine* machine = new Machine();
		Compiler* compiler = new Compiler();
		Program* program = compiler->Compile(srcpath, dstpath);
		program->SaveBytecode(dstpath);
		delete program;
		delete compiler;
		delete machine;
		return 0;
	}

	if (option == "-cr") {
		if (dstpath.empty()) {
			TUtil::Error("Missing destination filename");
			return 1;
		}
		Machine* machine = new Machine();
		Compiler* compiler = new Compiler();
		Program* program = compiler->Compile(srcpath, dstpath);
		program->SaveBytecode(dstpath);
		machine->Run(program);
		delete program;
		delete compiler;
		delete machine;
		return 0;
	}

	return 0;
}
