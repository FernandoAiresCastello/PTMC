#include "Program.h"

#define BYTECODE_LENGTH 0x10000

Program::Program()
{
	ClearBytecode();
}

Program::~Program()
{
}

void Program::ClearBytecode()
{
	Bytecode.clear();
	for (int i = 0; i < BYTECODE_LENGTH; i++)
		Bytecode.push_back(0);
}

void Program::LoadBytecode(std::string filename)
{
	ClearBytecode();
	std::vector<int> compiled = TFile::ReadBytes(filename);
	for (int i = 0; i < BYTECODE_LENGTH && i < compiled.size(); i++) {
		Bytecode[i] = compiled[i];
	}
}

void Program::SaveBytecode(std::string filename)
{
	std::vector<int> compiled;
	for (int i = 0; i < Bytecode.size(); i++)
		compiled.push_back(Bytecode[i]);

	TFile::WriteBytes(filename, compiled);
	SaveBytecodeAsPlainText(filename);
}

void Program::SaveBytecodeAsPlainText(std::string filename)
{
	filename += ".txt";

	std::string text;
	int countPerRow = 0;

	for (int i = 0; i < Bytecode.size(); i++) {
		std::string hex = TString::Format("%02X ", Bytecode[i]);
		text += hex;
		countPerRow++;
		if (countPerRow > 15) {
			countPerRow = 0;
			text += "\n";
		}
	}

	TFile::WriteText(filename, text);
}
