#include "Compiler.h"
#include "Command.h"

struct LabelDest {
	std::string Name;
	int Address;
};

struct LabelOrig {
	std::string Name;
	int Address;
};

Compiler::Compiler()
{
}

Compiler::~Compiler()
{
}

void Compiler::Abort(std::string msg)
{
	TUtil::Error(msg);
	exit(1);
}

std::vector<std::string> Compiler::LoadSource(std::string filename)
{
	return TFile::ReadLines(filename);
}

Program* Compiler::Compile(std::string srcfile, std::string dstfile)
{
	std::vector<byte> bytecode;
	auto lines = LoadSource(srcfile);
	unsigned long progAddr = 0;

	for (int srcln = 0; srcln < lines.size(); srcln++) {
		std::string line = TString::Trim(lines[srcln]);
		if (!line.empty()) {
			if (TString::StartsWith(line, ';')) {
				// It's a comment
				continue;
			}
			if (TString::EndsWith(line, ':')) {
				// It's a label
				std::string labelName = TString::RemoveLast(line);
				LabelDest label;
				label.Name = labelName;
				label.Address = progAddr;
				LabelDestAddr.push_back(label);
				continue;
			}

			auto bytes = CompileLine(line, srcln, progAddr);
			bytecode.insert(bytecode.end(), bytes.begin(), bytes.end());
			progAddr = bytecode.size();
		}
	}

	ResolveLabels(bytecode);

	Program* program = new Program();
	program->Bytecode = bytecode;
	return program;
}

std::vector<byte> Compiler::CompileLine(std::string line, int srcln, int progAddr)
{
	std::vector<byte> bytes;
	std::vector<byte> byteParams;
	std::vector<std::string> stringParams;
	std::string cmd;
	int ixFirstSpace = line.find_first_of(' ');
	
	if (ixFirstSpace >= 0) {
		cmd = line.substr(0, ixFirstSpace);
		std::string rawParamStr = TString::Trim(line.substr(ixFirstSpace));
		stringParams = TString::Split(rawParamStr, ' ');
		for (auto& rawParam : stringParams) {
			std::string param = TString::Trim(rawParam);
			if (!param.empty()) {
				byteParams.push_back(TString::ToInt(rawParam));
			}
		}
	}
	else {
		cmd = line;
	}

	cmd = TString::ToUpper(cmd);

	if (Command::Name.find(cmd) == Command::Name.end()) {
		bytes.clear();
		Abort(TString::Format(
			"Compilation error:\n\nInvalid command %s at line %i", cmd.c_str(), srcln + 1));
		
		return bytes;
	}

	bytes.push_back(Command::Name[cmd]);
	
	if (cmd == "GOTO") {
		LabelOrig label;
		label.Name = stringParams[0];
		label.Address = progAddr + 1;
		LabelOrigAddr.push_back(label);
		// Add placeholder for label address
		bytes.push_back(0);
		bytes.push_back(0);
	}
	else {
		if (!byteParams.empty())
			bytes.insert(bytes.end(), byteParams.begin(), byteParams.end());
	}

	return bytes;
}

void Compiler::ResolveLabels(std::vector<byte>& program)
{
	for (auto& origLabel : LabelOrigAddr) {
		for (auto& destLabel : LabelDestAddr) {
			if (origLabel.Name == destLabel.Name) {
				byte destAddrBytes[2];
				TUtil::ShortToBytes(destLabel.Address, destAddrBytes);
				program[origLabel.Address + 0] = destAddrBytes[1];
				program[origLabel.Address + 1] = destAddrBytes[0];
			}
		}
	}
}
