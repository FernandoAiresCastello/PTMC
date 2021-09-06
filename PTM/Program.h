#pragma once
#include <map>
#include <vector>
#include <string>
#include <TileGameLib.h>
using namespace TileGameLib;

class Program
{
public:
	Program();
	~Program();

	std::vector<byte> Bytecode;

	void LoadBytecode(std::string filename);
	void SaveBytecode(std::string filename);
	void SaveBytecodeAsPlainText(std::string filename);

private:
	Program(const Program& other) = delete;
};
