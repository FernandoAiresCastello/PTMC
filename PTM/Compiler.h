#pragma once
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

class Compiler
{
public:
	Compiler();
	~Compiler();

	std::vector<byte> Compile(std::string filename);

private:
	Compiler(const Compiler& other) = delete;

	std::vector<std::string> LoadSource(std::string filename);
	void SaveCompiled(std::string filename, std::vector<byte>& bytecode);
	std::vector<byte> CompileLine(std::string line, int srcln);
	void Abort(std::string msg);
};
