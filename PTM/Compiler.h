#pragma once
#include "Program.h"
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

struct LabelDest;
struct LabelOrig;

class Compiler
{
public:
	Compiler();
	~Compiler();

	Program* Compile(std::string srcfile, std::string dstfile);

private:
	std::vector<LabelDest> LabelDestAddr;
	std::vector<LabelOrig> LabelOrigAddr;

	void Abort(std::string msg);
	std::vector<std::string> LoadSource(std::string filename);
	std::vector<byte> CompileLine(std::string line, int srcln, int progAddr);
	void ResolveLabels(std::vector<byte>& bytecode);
};
