#pragma once
#include "Program.h"
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

struct LabelDest;
struct LabelOrig;
struct DataItem;

class Compiler
{
public:
	Compiler();
	~Compiler();

	Program* Compile(std::string srcfile, std::string dstfile);

private:
	std::vector<LabelDest> LabelDestAddr;
	std::vector<LabelOrig> LabelOrigAddr;
	std::map<std::string, std::string> ConstDefs;

	void Cancel(int line);
	void Abort(std::string msg, int line = -1);
	std::vector<std::string> LoadSource(std::string filename);
	std::vector<byte> CompileLine(Program* program, std::string line, int srcln, int progAddr);
	void ResolveLabels(std::vector<byte>& bytecode);
	void ReplaceConstants(Program* program, std::vector<std::string>& sourceLines);
};
