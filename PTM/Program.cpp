#include "Program.h"

Program::Program()
{
}

Program::~Program()
{
}

void Program::LoadBytecode(std::string filename)
{
	Bytecode.clear();
	std::vector<int> compiled = TFile::ReadBytes(filename);
	for (int& byte : compiled) {
		Bytecode.push_back(byte);
	}
}

void Program::SaveBytecode(std::string filename)
{
	std::vector<int> compiled;
	for (byte& byte : Bytecode) {
		compiled.push_back(byte);
	}
	TFile::WriteBytes(filename, compiled);
}
