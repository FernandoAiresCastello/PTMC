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

void Compiler::Abort(std::string msg, int line)
{
	TUtil::Error(TString::Format(
		"Compilation error:\n\n%s at line %i", msg.c_str(), line + 1));

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
	int numericParam;
	std::string stringParam;
	std::string cmd;
	int ixFirstSpace = line.find_first_of(' ');
	bool hasParam = false;
	
	if (ixFirstSpace >= 0) {
		cmd = TString::ToUpper(line.substr(0, ixFirstSpace));
		std::string rawParam = TString::Trim(line.substr(ixFirstSpace));
		stringParam = TString::Trim(rawParam);

		if (!stringParam.empty()) {
			int number = TString::ToInt(rawParam);
			if (number < 0) {
				bytes.clear();
				Abort("Numeric param underflow", srcln);
				return bytes;
			}
			if (number > 65535) {
				bytes.clear();
				Abort("Numeric param overflow", srcln);
				return bytes;
			}

			if (cmd == "PUSH" && number > 255)
				cmd = "PUSHW";

			numericParam = number;
			hasParam = true;
		}
	}
	else {
		cmd = TString::ToUpper(line);
	}

	if (Command::Name.find(cmd) == Command::Name.end()) {
		bytes.clear();
		Abort(TString::Format("Invalid command %s", cmd.c_str()), srcln);
		return bytes;
	}

	bytes.push_back(Command::Name[cmd]);
	
	if (hasParam) {
		if (cmd == "GOTO") {
			LabelOrig label;
			label.Name = stringParam;
			label.Address = progAddr + 1;
			LabelOrigAddr.push_back(label);
			// Add placeholder for label address
			bytes.push_back(0);
			bytes.push_back(0);
		}
		else {
			if (numericParam > 255 && numericParam < 65536) {
				byte nibbles[2];
				TUtil::ShortToBytes(numericParam, nibbles);
				bytes.push_back(nibbles[1]);
				bytes.push_back(nibbles[0]);
			}
			else {
				bytes.push_back(numericParam);
			}
		}
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
