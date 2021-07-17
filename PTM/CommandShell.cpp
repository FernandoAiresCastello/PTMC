#include <algorithm>
#include "CommandShell.h"

#define ERR_SYNTAX_ERROR "Syntax error"

CommandShell::CommandShell(Graphics* gr, Datafile* data)
	: EditorBase(gr, data)
{
}

CommandShell::~CommandShell()
{
}


void CommandShell::OnStart()
{
	PrintIntro();
}

void CommandShell::TypeEnter()
{
	std::string type = "";

	if (HasObject(Cursor->GetX(), Cursor->GetY()))
		type = GetObjectType(Cursor->GetX(), Cursor->GetY());

	if (type == "" || type == "plain") {
		Crlf();
		std::string line = GetStringInLine(Cursor->GetY() - 1);
		if (!line.empty()) {
			InterpretLine(line);
		}
	}
	else {
		// todo
	}
}

void CommandShell::PrintIntro()
{
	TypeStringCrlf("PTM 1.0");
	Ok();
}

void CommandShell::Ok()
{
	TypeStringCrlf("Ok");
}

void CommandShell::InterpretLine(std::string line)
{
	int ixFirstSpace = line.find(' ');
	std::string cmd = String::ToLower(ixFirstSpace >= 0 ? line.substr(0, ixFirstSpace) : line);
	std::string rawParams = ixFirstSpace >= 0 ? line.substr(ixFirstSpace) : "";
	auto params = String::Split(rawParams, ',');
	std::string error = "";

	if (cmd == "exit") {
		Running = false;
		return;
	}
	else if (cmd == "fgcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetForeColor(String::ToInt(params[0]));
		}
	}
	else if (cmd == "bgcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetBackColor(String::ToInt(params[0]));
		}
	}
	else if (cmd == "bdcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetBorderColor(String::ToInt(params[0]));
		}
	}
	else {
		error = ERR_SYNTAX_ERROR;
	}

	if (!error.empty())
		TypeStringCrlf(error);

	Ok();
}
