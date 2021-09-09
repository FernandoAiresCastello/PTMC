#include "Command.h"
#include "Machine.h"
#include "Compiler.h"
#include "Program.h"
#include <TileGameLib.h>
#include <CppUtils/CppUtils.h>

using namespace TileGameLib;
using namespace CppUtils;

int main(int argc, char* argv[])
{
	if (argc < 3) {
		MsgBox::Error("Invalid command line. Valid syntax:\n\n"
			"Run:\nptm -r <file>\n\n"
			"Compile:\nptm -c <srcfile> <dstfile>\n\n"
			"Compile then run:\nptm -cr <srcfile> <dstfile>");

		return 1;
	}

	std::string option = argv[1];
	std::string srcpath = argv[2];
	std::string dstpath = argc > 3 ? argv[3] : "";

	if (!TFile::Exists(srcpath)) {
		MsgBox::Error(String::Format("File not found: %s", srcpath.c_str()));
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
			MsgBox::Error("Missing destination filename");
			return 1;
		}
		Compiler* compiler = new Compiler();
		Program* program = compiler->Compile(srcpath, dstpath);
		if (program != nullptr) {
			program->SaveBytecode(dstpath);
			delete program;
		}
		delete compiler;
		return program == nullptr ? 1 : 0;
	}

	if (option == "-cr") {
		if (dstpath.empty()) {
			MsgBox::Error("Missing destination filename");
			return 1;
		}
		Compiler* compiler = new Compiler();
		Program* program = compiler->Compile(srcpath, dstpath);
		if (program != nullptr) {
			program->SaveBytecode(dstpath);
			Machine* machine = new Machine();
			machine->Run(program);
			delete machine;
			delete program;
		}
		delete compiler;
		return program == nullptr ? 1 : 0;
	}

	return 0;
}
