#pragma once
#include <map>
#include <vector>
#include <string>
#include <TileGameLib.h>
#include <CppUtils/CppUtils.h>

using namespace TileGameLib;
using namespace CppUtils;

class Program
{
public:
	Program();
	~Program();

	std::vector<byte> Bytecode;

	void LoadBytecode(std::string filename);
	void SaveBytecode(std::string filename);
	void SaveBytecodeAsPlainText(std::string filename);
};
