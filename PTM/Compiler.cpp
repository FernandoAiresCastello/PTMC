#include <CppUtils/CppUtils.h>
#include "Compiler.h"
#include "Command.h"

using namespace CppUtils;

//=============================================================================
//	Symbol definitions
//=============================================================================

#define SYM_SEPARATOR	' '
#define SYM_DIRECTIVE	'.'
#define SYM_DATA_PTR	'&'
#define SYM_COMMENT		';'
#define SYM_LABEL		':'
#define SYM_VARIABLE	'$'

//=============================================================================
//	Constants
//=============================================================================

#define BYTE_MAX	255
#define WORD_MAX	65535

//=============================================================================
//	Support structures
//=============================================================================

struct LabelDest {
	std::string Name;
	int Address;
};

struct LabelOrig {
	std::string Name;
	int Address;
};

struct DataItem {
	std::string Id;
	std::vector<byte> Data;
	int Address;
};

//=============================================================================
//	Basic functions
//=============================================================================

Compiler::Compiler()
{
}

Compiler::~Compiler()
{
}

void Compiler::Abort(std::string msg, int line)
{
	std::string fmt = "";

	if (line >= 0)
		fmt = String::Format("Compilation error:\n\n%s at line %i", msg.c_str(), line + 1);
	else
		fmt = String::Format("Compilation error:\n\n%s", msg.c_str());

	Util::Error(fmt);
	exit(1);
}

//=============================================================================
//	Bytecode compilation
//=============================================================================

std::vector<std::string> Compiler::LoadSource(std::string filename)
{
	return TFile::ReadLines(filename);
}

Program* Compiler::Compile(std::string srcfile, std::string dstfile)
{
	Program* program = new Program();
	auto lines = LoadSource(srcfile);
	CompileData(program, lines);
	unsigned long progAddr = 0;

	for (int srcln = 0; srcln < lines.size(); srcln++) {
		std::string line = String::Trim(lines[srcln]);
		if (!line.empty()) {
			if (String::StartsWith(line, SYM_COMMENT)) {
				// It's a comment
				continue;
			}
			if (String::EndsWith(line, SYM_LABEL)) {
				// It's a label
				std::string labelName = String::RemoveLast(line);
				LabelDest label;
				label.Name = labelName;
				label.Address = progAddr;
				LabelDestAddr.push_back(label);
				continue;
			}

			auto bytes = CompileLine(program, line, srcln, progAddr);
			for (byte& b : bytes) {
				program->Bytecode[progAddr] = b;
				progAddr++;
			}
		}
	}

	ResolveLabels(program->Bytecode);

	return program;
}

std::vector<byte> Compiler::CompileLine(Program* program, std::string line, int srcln, int progAddr)
{
	std::vector<byte> bytes;
	int numericParam;
	std::string stringParam;
	std::string cmd;
	int ixFirstSpace = line.find_first_of(SYM_SEPARATOR);
	bool hasParam = false;
	
	if (ixFirstSpace >= 0) {
		cmd = String::ToUpper(line.substr(0, ixFirstSpace));
		stringParam = String::Trim(line.substr(ixFirstSpace));

		if (!stringParam.empty()) {

			if (String::StartsWith(stringParam, SYM_DATA_PTR)) {
				std::string id = String::Skip(stringParam, 1);
				if (DataPtr.find(id) != DataPtr.end()) {
					byte data = program->Bytecode[DataPtr[id]];
					stringParam = String::ToString(data);
				}
				else {
					bytes.clear();
					Abort(String::Format("Undefined data id %s", id.c_str()), srcln);
					return bytes;
				}
			}

			int number = String::ToInt(stringParam);
			if (number < 0) {
				bytes.clear();
				Abort("Numeric underflow", srcln);
				return bytes;
			}
			if (number > WORD_MAX) {
				bytes.clear();
				Abort("Numeric overflow", srcln);
				return bytes;
			}

			if (cmd == "PUSH" && number > BYTE_MAX)
				cmd = "PUSHW";

			numericParam = number;
			hasParam = true;
		}
	}
	else {
		cmd = String::ToUpper(line);
	}

	if (Command::Name.find(cmd) == Command::Name.end()) {
		bytes.clear();
		Abort(String::Format("Invalid command %s", cmd.c_str()), srcln);
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
			if (numericParam > BYTE_MAX && numericParam <= WORD_MAX) {
				byte nibbles[2];
				Util::ShortToBytes(numericParam, nibbles);
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
				Util::ShortToBytes(destLabel.Address, destAddrBytes);
				program[origLabel.Address + 0] = destAddrBytes[1];
				program[origLabel.Address + 1] = destAddrBytes[0];
			}
		}
	}
}

//=============================================================================
//	Data compilation
//=============================================================================

void Compiler::CompileData(Program* program, std::vector<std::string>& sourceLines)
{
	Data.clear();
	DataPtr.clear();
	DataDirectoryAddress = -1;

	// STEP 1: find and process data directives
	for (int srcLineNumber = 0; srcLineNumber < sourceLines.size(); srcLineNumber++) {
		std::string srcLine = sourceLines[srcLineNumber];
		std::string line = String::Trim(srcLine);

		if (String::StartsWith(line, SYM_DIRECTIVE)) {

			sourceLines[srcLineNumber] = "";

			int ixFirstSpace = line.find_first_of(SYM_SEPARATOR);
			int ixSecondSpace = line.find_first_of(SYM_SEPARATOR, ixFirstSpace + 1);

			std::string directive = String::Trim(String::ToLower(line.substr(1, ixFirstSpace - 1)));
			
			if (directive == "data") {
				std::string address = String::Trim(line.substr(ixFirstSpace));
				DataDirectoryAddress = String::ToInt(address);
				continue;
			}
			if (ixFirstSpace <= 0 || ixSecondSpace <= 0) {
				Abort("Invalid data directive", srcLineNumber);
				return;
			}
			if (DataDirectoryAddress < 0) {
				Abort("Undefined data directory address");
				return;
			}

			std::string id = String::Trim(line.substr(ixFirstSpace + 1, ixSecondSpace - ixFirstSpace));
			std::string data = String::Trim(line.substr(ixSecondSpace));

			if (directive == "byte") {
				Data.push_back(CompileByteData(id, data, srcLineNumber));
			}
			else if (directive == "text") {
				Data.push_back(CompileTextData(id, data, srcLineNumber));
			}
			else {
				Abort(String::Format("Invalid data directive .%s", directive.c_str()), srcLineNumber);
				return;
			}
		}
	}

	// STEP 2: Insert compiled data bytes into bytecode
	for (auto& item : Data) {
		std::string id = item.Id;
		DataPtr[id] = DataDirectoryAddress;
		for (auto& byte : item.Data) {
			program->Bytecode[DataDirectoryAddress] = byte;
			DataDirectoryAddress++;
		}
	}
}

DataItem Compiler::CompileByteData(std::string id, std::string data, int srcln)
{
	DataItem item;
	item.Id = id;
	item.Address = 0;

	auto bytes = String::Split(data, SYM_SEPARATOR);
	for (auto& strByte : bytes) {
		int byte = String::ToInt(strByte);
		if (byte < 0 || byte > BYTE_MAX) {
			Abort("Invalid byte value", srcln);
			return item;
		}
		else {
			item.Data.push_back(byte);
		}
	}

	return item;
}

DataItem Compiler::CompileTextData(std::string id, std::string data, int srcln)
{
	DataItem item;
	item.Id = id;

	std::string text = String::RemoveFirstAndLast(data);
	for (char& ch : text)
		item.Data.push_back(ch);

	item.Data.push_back(0);
	return item;
}
