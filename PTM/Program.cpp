#include "Program.h"

Program::Program()
{
}

Program::~Program()
{
}

void Program::LoadBytecode(std::string filename)
{
	std::vector<int> file = TFile::ReadBytes(filename);
	Bytecode.clear();
	for (int i = 0; i < file.size(); i++)
		Bytecode.push_back(file[i]);
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
		std::string hex = String::Format("%02X ", Bytecode[i]);
		text += hex;
		countPerRow++;
		if (countPerRow > 15) {
			countPerRow = 0;
			text += "\n";
		}
	}

	TFile::WriteText(filename, text);
}
