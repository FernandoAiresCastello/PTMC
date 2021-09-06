#include "Compiler.h"
#include "Machine.h"

Compiler::Compiler()
{
}

Compiler::~Compiler()
{
}

std::vector<std::string> Compiler::LoadSource(std::string filename)
{
	return TFile::ReadLines(filename);
}

Program* Compiler::Compile(std::string srcfile, std::string dstfile)
{
	std::vector<byte> bytecode;
	std::map<std::string, unsigned long> labels;
	auto lines = LoadSource(srcfile);
	unsigned long programIndex = 0;

	for (int srcln = 0; srcln < lines.size(); srcln++) {
		std::string line = TString::Trim(lines[srcln]);
		if (!line.empty()) {
			if (TString::StartsWith(line, ';')) {
				continue;
			}
			if (TString::EndsWith(line, ':')) {
				std::string label = TString::RemoveLast(line);
				labels[label] = programIndex;
				continue;
			}

			auto bytes = CompileLine(line, srcln);
			bytecode.insert(bytecode.end(), bytes.begin(), bytes.end());
			programIndex = bytecode.size();
		}
	}

	Program* program = new Program();
	program->Bytecode = bytecode;
	return program;
}

std::vector<byte> Compiler::CompileLine(std::string line, int srcln)
{
	std::vector<byte> bytes;
	std::vector<byte> params;
	std::string cmd;
	int ixFirstSpace = line.find_first_of(' ');
	
	if (ixFirstSpace >= 0) {
		cmd = line.substr(0, ixFirstSpace);
		std::string rawParamStr = TString::Trim(line.substr(ixFirstSpace));
		auto rawParams = TString::Split(rawParamStr, ' ');
		for (auto& rawParam : rawParams) {
			std::string param = TString::Trim(rawParam);
			if (!param.empty()) {
				params.push_back(TString::ToInt(rawParam));
			}
		}
	}
	else {
		cmd = line;
	}

	cmd = TString::ToUpper(cmd);

	if (CmdMap.find(cmd) == CmdMap.end()) {
		bytes.clear();
		Abort(TString::Format("Compilation error:\n\nInvalid command %s at line %i", 
			cmd.c_str(), srcln + 1));
		
		return bytes;
	}

	bytes.push_back(CmdMap[cmd]);
	if (!params.empty())
		bytes.insert(bytes.end(), params.begin(), params.end());

	return bytes;
}

void Compiler::Abort(std::string msg)
{
	TUtil::Error(msg);
	exit(1);
}
