#pragma once
#include "Program.h"
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

class Compiler
{
public:
	Compiler();
	~Compiler();

	Program* Compile(std::string srcfile, std::string dstfile);

private:
	std::vector<std::string> LoadSource(std::string filename);
	std::vector<byte> CompileLine(std::string line, int srcln);
	void Abort(std::string msg);
};
