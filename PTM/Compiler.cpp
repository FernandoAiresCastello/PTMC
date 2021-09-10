#include <CppUtils/CppUtils.h>
#include "Compiler.h"
#include "Command.h"

using namespace CppUtils;

//=============================================================================
//	Symbol definitions
//=============================================================================

#define SYM_CANCEL		'!'
#define SYM_SEPARATOR	' '
#define SYM_DIRECTIVE	'.'
#define SYM_STRING		'"'
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
//	Macros
//=============================================================================

#define ABORT_COMPILATION(msg) Abort(msg, srcln); bytecode.clear(); return bytecode;
#define BREAK_ON(cmd) if (command == cmd) SDL_TriggerBreakpoint();


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

void Compiler::Cancel(int line)
{
	MsgBox::Warning(String::Format("Compilation cancelled at line %i", line + 1));
	exit(0);
}

void Compiler::Abort(std::string msg, int line)
{
	std::string fmt = "";

	if (line >= 0)
		fmt = String::Format("Compilation error:\n\n%s at line %i", msg.c_str(), line + 1);
	else
		fmt = String::Format("Compilation error:\n\n%s", msg.c_str());

	MsgBox::Error(fmt);
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
	auto lines = LoadSource(srcfile);
	Program* program = new Program();
	CompileData(program, lines);
	unsigned long progAddr = 0;

	for (int srcln = 0; srcln < lines.size(); srcln++) {
		std::string line = String::Trim(lines[srcln]);
		if (!line.empty()) {
			if (line[0] == SYM_CANCEL) {
				Cancel(srcln);
				return nullptr;
			}
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
				program->Bytecode.push_back(b);
				progAddr++;
			}
		}
	}

	ResolveLabels(program->Bytecode);

	return program;
}

std::vector<byte> Compiler::CompileLine(Program* program, std::string line, int srcln, int progAddr)
{
	std::vector<byte> bytecode;
	std::string command;
	std::string identifier;
	std::string stringLiteral;
	std::vector<int> numericParams;

	int ixFirstSpace = line.find_first_of(SYM_SEPARATOR);
	if (ixFirstSpace >= 0) {
		command = String::ToUpper(line.substr(0, ixFirstSpace));
		std::string strParams = String::Trim(line.substr(ixFirstSpace));
		// Parse parameters
		if (!strParams.empty()) {
			if (String::StartsAndEndsWith(strParams, SYM_STRING)) {
				// It's a string literal
				stringLiteral = String::RemoveFirstAndLast(strParams);
			}
			else if (isalpha(strParams[0])) {
				// It's an identifier
				identifier = strParams;
			}
			else {
				// It's numbers
				for (auto& strParam : String::Split(strParams, SYM_SEPARATOR)) {
					numericParams.push_back(String::ToInt(strParam));
				}
			}
		}
	}
	else {
		command = String::ToUpper(line);
	}

	if (Command::Name.find(command) == Command::Name.end()) {
		ABORT_COMPILATION(String::Format("Invalid command %s", command.c_str()));
	}

	bytecode.push_back(Command::Name[command]);
	
	// === GOTO ===
	if (command == "GOTO") {
		if (identifier.empty()) {
			ABORT_COMPILATION("Label identifier expected");
		}
		LabelOrig label;
		label.Name = identifier;
		label.Address = progAddr + 1;
		LabelOrigAddr.push_back(label);
		// Add placeholder for label address
		bytecode.push_back(0);
		bytecode.push_back(0);
	}
	// === STS ===
	else if (command == "STS") {
		int count = numericParams.size();
		if (count != 1) {
			ABORT_COMPILATION("Pointer expected");
		}
		byte nibbles[2];
		Util::ShortToBytes(numericParams[0], nibbles);
		bytecode.push_back(nibbles[1]);
		bytecode.push_back(nibbles[0]);
	}
	// === PUSH / PUSHA ===
	else if (command == "PUSH" || command == "PUSHA") {
		int count = numericParams.size();
		if (count == 0) {
			ABORT_COMPILATION("Parameters expected");
		}
		if (command == "PUSHA") {
			if (count > BYTE_MAX) {
				ABORT_COMPILATION("Parameter list exceeds 255 items");
			}
			bytecode.push_back(count);
		}
		else if (command == "PUSH" && count > 1) {
			ABORT_COMPILATION("Single parameter expected");
		}
		for (auto& param : numericParams) {
			byte sign = param < 0 ? 1 : 0;
			byte nibbles[2];
			int value = abs(param);
			if (value > WORD_MAX) {
				ABORT_COMPILATION("Parameter overflow");
			}
			Util::ShortToBytes(value, nibbles);
			bytecode.push_back(sign);
			bytecode.push_back(nibbles[1]);
			bytecode.push_back(nibbles[0]);
		}
	}
	// === PUSHS ===
	else if (command == "PUSHS") {
		bytecode.push_back(stringLiteral.length());
		for (int i = stringLiteral.length() - 1; i >= 0; i--) {
			char ch = stringLiteral[i];
			if (ch < 0 || ch > 255) {
				ABORT_COMPILATION("String character overflow");
			}
			bytecode.push_back(ch);
		}
	}
	// === All other commands ===
	else {
		bytecode.insert(bytecode.end(), numericParams.begin(), numericParams.end());
	}

	return bytecode;
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
