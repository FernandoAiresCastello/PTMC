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
