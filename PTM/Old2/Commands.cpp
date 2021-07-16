#include <map>
#include <stack>
#include <vector>
#include "Commands.h"
#include "TileMachine.h"
#include "SharedScreen.h"
#include "SharedObjects.h"
#include "GameData.h"
#include <TBRLGPT.h>
using namespace TBRLGPT;

std::map<std::string, void(*)()> Cmd;

void InitCommandTable()
{
	Cmd["NOP"] = &_0x00;
	Cmd["PUSH"] = &_0x01;
	Cmd["POP"] = &_0x02;
	Cmd["REFR"] = &_0x07;
	Cmd["ADD"] = &_0x08;
	Cmd["SUB"] = &_0x09;
	Cmd["JP"] = &_0x0a;
	Cmd["WAIT"] = &_0x0b;
	Cmd["BRK"] = &_0xfd;
	Cmd["HALT"] = &_0xfe;
	Cmd["EXIT"] = &_0xff;
}

void _0x00()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	// no operation
}

void _0x01()
{
	if (!Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	PUSH(Machine->Line->ParamNumber);
}

void _0x02()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine->Stack.empty()) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	Machine->Stack.pop();
}

void _0xfd()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	__debugbreak();
}

void _0xfe()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine->Halted = true;
}

void _0xff()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Machine->Running = false;
}

void _0x07()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	RefreshMachineScreen();
}

void _0x08()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine->Stack.size() < 2) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	int b = POP;
	int a = POP;
	PUSH(a + b);
}

void _0x09()
{
	if (Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}
	if (Machine->Stack.size() < 2) {
		SetError(ERR_STACK_EMPTY);
		return;
	}

	int b = POP;
	int a = POP;
	PUSH(a - b);
}

void _0x0a()
{
	if (!Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	BranchTo(Machine->Line->ParamString);
}

void _0x0b()
{
	if (!Machine->Line->HasParam) {
		SetError(ERR_SYNTAX_ERROR);
		return;
	}

	Util::Pause(Machine->Line->ParamNumber);
}
