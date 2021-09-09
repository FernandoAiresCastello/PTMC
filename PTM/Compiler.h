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
	std::vector<DataItem> Data;
	std::map<std::string, int> DataPtr;
	int DataDirectoryAddress = 0;

	void Cancel(int line);
	void Abort(std::string msg, int line = -1);
	std::vector<std::string> LoadSource(std::string filename);
	std::vector<byte> CompileLine(Program* program, std::string line, int srcln, int progAddr);
	void ResolveLabels(std::vector<byte>& bytecode);
	void CompileData(Program* program, std::vector<std::string>& sourceLines);
	DataItem CompileByteData(std::string id, std::string data, int srcln);
	DataItem CompileTextData(std::string id, std::string data, int srcln);
};
