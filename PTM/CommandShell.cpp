#include <algorithm>
#include "CommandShell.h"
#include "CharEdit.h"

#define ERR_SYNTAX_ERROR "Syntax error"

CommandShell::CommandShell(Graphics* gr, Datafile* data) : EditorBase(gr, data)
{
	ProgEditor = new ProgramEditor(gr, data);
	CharEdit::X = Gr->Cols / 2 - 5;
	CharEdit::Y = Gr->Rows / 2 - 5;
}

CommandShell::~CommandShell()
{
	delete ProgEditor;
}

void CommandShell::OnStart()
{
	PrintIntro();
}

void CommandShell::OnLoop()
{
	SceneObject* o = GetObjectUnderCursor();
	if (o == NULL) {
		BottomBorderText = "Null";
		return;
	}

	const std::string type = GetObjectType(Cursor->GetX(), Cursor->GetY());

	if (type == "")
		BottomBorderText = "No type";
	else if (type == "plain")
		BottomBorderText = "Plain char";
	else if (type == "charset")
		BottomBorderText = String::Format("Charset:%i", GetObjectFrame(o, 0).Index);
	else if (type == "palette")
		BottomBorderText = String::Format("Palette:%i", GetObjectFrame(o, 0).ForeColorIx);
	else
		BottomBorderText = "Unrecognized type: " + type;
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
	else if (type == "charset") {
		int ch = GetObjectFrame(GetObjectUnderCursor(), 0).Index;
		CharEdit* editor = new CharEdit(this, Gr, Data, ch);
		editor->Running = true;
		while (editor->Running) {
			Draw();
			editor->Draw();
			Gr->Update();
			editor->HandleEvents();
		}
		delete editor;
	}
	else if (type == "palette") {
		// todo
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
	else if (cmd == "color") {
		if (params.size() != 3) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetForeColor(String::ToInt(params[0]));
			SetBackColor(String::ToInt(params[1]));
			SetBorderColor(String::ToInt(params[2]));
		}
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
	else if (cmd == "prog") {
		ProgEditor->SetForeColor(ForeColor);
		ProgEditor->SetBackColor(BackColor);
		ProgEditor->SetBorderColor(BorderColor);
		ProgEditor->Run();
	}
	else if (cmd == "char") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			PrintCharset(String::ToInt(params[0]), String::ToInt(params[0]));
		}
	}
	else if (cmd == "charset") {
		if (params.size() != 2) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			PrintCharset(String::ToInt(params[0]), String::ToInt(params[1]));
		}
	}
	else if (cmd == "pal") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			PrintPalette(String::ToInt(params[0]), String::ToInt(params[0]));
		}
	}
	else if (cmd == "palette") {
		if (params.size() != 2) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			PrintPalette(String::ToInt(params[0]), String::ToInt(params[1]));
		}
	}
	else if (cmd == "setpal") {
		if (params.size() != 4) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			int i = String::ToInt(params[0]);
			int r = String::ToInt(params[1]);
			int g = String::ToInt(params[2]);
			int b = String::ToInt(params[3]);
			Data->GetPalette()->Set(i, r, g, b);
		}
	}
	else {
		error = ERR_SYNTAX_ERROR;
	}

	if (!error.empty())
		TypeStringCrlf(error);

	Ok();
}

void CommandShell::PrintCharset(int first, int last)
{
	for (int i = first; i <= last; i++) {
		Object o;
		o.GetAnimation().Clear();
		o.GetAnimation().AddFrame(ObjectChar(i, ForeColor, BackColor));
		o.SetProperty("type", "charset");
		TypeObject(o);
		if (Cursor->GetX() >= View->GetWidth() - 1) {
			Crlf();
		}
	}
	Crlf();
}

void CommandShell::PrintPalette(int first, int last)
{
	for (int i = first; i <= last; i++) {
		Object o;
		o.GetAnimation().Clear();
		o.GetAnimation().AddFrame(ObjectChar(0, i, i));
		o.SetProperty("type", "palette");
		TypeObject(o);
		if (Cursor->GetX() >= View->GetWidth() - 1) {
			Crlf();
		}
	}
	Crlf();
}
