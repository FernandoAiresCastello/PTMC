#include <algorithm>
#include "CommandShell.h"
#include "CharEdit.h"

#define ERR_SYNTAX_ERROR "Syntax error"
#define ERR_TEMPLATE_NOT_FOUND "Template not found"

CommandShell::CommandShell(Graphics* gr, Datafile* data) : EditorBase(gr, data)
{
	ProgEditor = new ProgramEditor(gr, data);
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
		BottomBorderText = "";
		return;
	}

	const std::string type = GetObjectType(Cursor->GetX(), Cursor->GetY());
	const ObjectChar& ch = GetObjectFrame(o, 0);

	if (type == "")
		BottomBorderText = "No type";
	else if (type == "char")
		BottomBorderText = String::Format("Char:%i", ch.Index);
	else if (type == "tile")
		BottomBorderText = String::Format("Tile:%i,%i,%i", ch.Index, ch.ForeColorIx, ch.BackColorIx);
	else if (type == "charset")
		BottomBorderText = String::Format("Charset:%i", ch.Index);
	else if (type == "palette")
		BottomBorderText = String::Format("Palette:%i (%02X %02X %02X)", ch.ForeColorIx, 
			Data->GetPalette()->Get(ch.ForeColorIx).R, 
			Data->GetPalette()->Get(ch.ForeColorIx).G, 
			Data->GetPalette()->Get(ch.ForeColorIx).B);
	else if (type == "template")
		BottomBorderText = String::Format("Template:%s", o->GetObj()->GetPropertyAsString("template_id").c_str());
	else
		BottomBorderText = String::Format("Invalid type (%s)", type.c_str());
}

void CommandShell::TypeEnter()
{
	std::string type = "";

	if (HasObject(Cursor->GetX(), Cursor->GetY()))
		type = GetObjectType(Cursor->GetX(), Cursor->GetY());

	if (type == "" || type == "char") {
		Crlf();
		std::string line = GetStringInLine(Cursor->GetY() - 1);
		if (!line.empty()) {
			InterpretLine(line);
		}
	}
	else if (type == "charset") {
		int ch = GetObjectFrame(GetObjectUnderCursor(), 0).Index;
		EditChar(Gr, Data, ch);
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
	else if (cmd == "btcolor") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetBorderTextColor(String::ToInt(params[0]));
		}
	}
	else if (cmd == "prog") {
		ProgEditor->SetForeColor(ForeColor);
		ProgEditor->SetBackColor(BackColor);
		ProgEditor->SetBorderColor(BorderColor);
		ProgEditor->Run();
	}
	else if (cmd == "editchar") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			Cursor->GetObj()->SetVisible(false);
			EditChar(Gr, Data, String::ToInt(params[0]));
			Cursor->GetObj()->SetVisible(true);
		}
	}
	else if (cmd == "selchar") {
		if (params.size() != 0) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			Cursor->GetObj()->SetVisible(false);
			SelectChar(Gr, Data);
			Cursor->GetObj()->SetVisible(true);
		}
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
	else if (cmd == "copytile") {
		if (params.size() != 3) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			int i = String::ToInt(params[0]);
			int fgc = String::ToInt(params[1]);
			int bgc = String::ToInt(params[2]);
			SetClipboardTile(i, fgc, bgc);
		}
	}
	else if (cmd == "pushtile") {
		if (params.size() != 3) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			int i = String::ToInt(params[0]);
			int fgc = String::ToInt(params[1]);
			int bgc = String::ToInt(params[2]);
			PushTile(i, fgc, bgc);
		}
	}
	else if (cmd == "tiles") {
		if (params.size() != 0) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			TypeTileStack();
		}
	}
	else if (cmd == "poptiles") {
		if (params.size() != 0) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			TileStack.clear();
		}
	}
	else if (cmd == "settp") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			SetObjTemplate(params[0]);
		}
	}
	else if (cmd == "deltp") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			std::string id = params[0];
			if (Data->GetTemplate(id) != NULL) {
				DeleteObjTemplate(params[0]);
			}
			else {
				error = ERR_TEMPLATE_NOT_FOUND;
			}
		}
	}
	else if (cmd == "templates") {
		if (params.size() != 0) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			PrintTemplates();
		}
	}
	else if (cmd == "tp") {
		if (params.size() != 1) {
			error = ERR_SYNTAX_ERROR;
		}
		else {
			std::string id = params[0];
			if (Data->GetTemplate(id) != NULL) {
				TypeTemplate(id);
				Crlf();
			}
			else {
				error = ERR_TEMPLATE_NOT_FOUND;
			}
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

void CommandShell::PrintTemplates()
{
	auto& templates = Data->GetTemplates();
	for (int i = 0; i < templates.size(); i++) {
		SceneObject* o = templates[i];
		TypeTemplate(o->GetObj()->GetPropertyAsString("temp_id"));
	}
	Crlf();
}
