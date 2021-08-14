#include <Windows.h>
#include "Machine.h"

#define CMD(cmd,fn) CmdMap[cmd] = &Machine::fn

Machine::Machine()
{
	Running = false;
	Halted = false;
	CurrentLine = nullptr;
	Win = nullptr;
	Pal = TPalette::Default;
	Chars = TCharset::Default;
	BackColor = 0;
	Board = nullptr;
	View = nullptr;
	BoardCursor.X = 0;
	BoardCursor.Y = 0;
	BoardCursor.Layer = 0;

	InitCommandMap();
}

Machine::~Machine()
{
	for (int i = 0; i < Program.size(); i++) {
		delete Program[i];
		Program[i] = nullptr;
	}
	Program.clear();

	for (auto& temp : ObjTemplates) {
		delete temp.second;
		temp.second = nullptr;
	}
	ObjTemplates.clear();

	delete Win;
}

void Machine::LoadProgram(std::string filename)
{
	Program.clear();
	Labels.clear();

	int sourceLineNumber = 1;
	int actualLineNumber = 0;

	for (auto& srcline : TFile::ReadLines(filename)) {
		std::string line = TString::Trim(srcline);
		if (!line.empty() && !TString::StartsWith(line, "//")) {
			if (TString::EndsWith(line, ':')) {
				Labels[TString::RemoveLast(line)] = actualLineNumber;
			}
			else {
				Program.push_back(new ProgramLine(sourceLineNumber, actualLineNumber, srcline));
				actualLineNumber++;
			}
		}
		sourceLineNumber++;
	}
}

void Machine::Run()
{
	ProgramPtr = 0;
	Running = true;

	while (Running) {
		if (View) {
			View->Draw();
			Win->Update();
		}

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Running = false;
			break;
		}
		else if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN && TKey::Alt() && Win) {
				Win->ToggleFullscreen();
				Win->Update();
			}
			else if (e.key.keysym.sym == 1073742048 /* BREAK key */) {
				Running = false;
				break;
			}
		}

		if (Halted) {
			continue;
		}

		CurrentLine = Program[ProgramPtr];
		auto fn = CmdMap[CurrentLine->Command];
		if (fn)
			(this->*fn)();
		else
			Abort("Unknown command");

		if (Running && !Halted) {
			ProgramPtr++;
			if (ProgramPtr >= Program.size()) {
				Abort("Program pointer past end of program", false);
			}
		}
	}
}

void Machine::Abort(std::string msg, bool showSource)
{
	Running = false;
	
	if (showSource) {
		msg = TString::Format("%s\n\n%i %s",
			msg.c_str(), CurrentLine->SourceLineNumber, CurrentLine->Source.c_str());
	}

	MessageBox(Win ? (HWND)Win->GetHandle() : NULL, 
		msg.c_str(), "Runtime error", MB_ICONERROR | MB_OK);
}

Parameter Machine::Pop()
{
	if (ParamStack.empty()) {
		Abort("Stack empty");
		return Parameter();
	}
	else {
		Parameter param = ParamStack.top();
		ParamStack.pop();
		return param;
	}
}

int Machine::PopNumber()
{
	return Pop().Number;
}

std::string Machine::PopString()
{
	return Pop().String;
}

void Machine::InitCommandMap()
{
	// === System ===
	CMD("NOP", C_Nop);
	CMD("EXIT", C_Exit);
	CMD("HALT", C_Halt);
	// === Stack ===
	CMD("PUSH", C_Push);
	CMD("DUP", C_DuplicateStackItem);
	// === Window ===
	CMD("WINDOW.OPEN", C_WindowOpen);
	CMD("WINDOW.CLEAR", C_WindowClear);
	CMD("WINDOW.UPDATE", C_WindowUpdate);
	// === Palette ===
	CMD("PAL.LOAD", C_PaletteLoad);
	CMD("PAL.CLEAR", C_PaletteClear);
	CMD("PAL.SET", C_PaletteSet);
	// === Charset ===
	CMD("CHR.LOAD", C_CharsetLoad);
	CMD("CHR.CLEAR", C_CharsetClear);
	CMD("CHR.SET", C_CharsetSet);
	// === Map ===
	CMD("MAP.LAYER.ADD", C_NotImplemented);
	CMD("MAP.CURSOR.SET", C_MapCursorSet);
	CMD("MAP.TILE.ADD", C_MapTileAdd);
	CMD("MAP.OBJT.PUT", C_MapObjectTemplatePut);
	CMD("MAP.OBJ.GRAB", C_NotImplemented);
	// === Object templates ===
	CMD("OBJT.LOAD", C_ObjectTemplatesLoad);
	CMD("OBJT.NEW", C_ObjectTemplateCreate);
	CMD("OBJT.TILE.ADD", C_ObjectTemplateTileAdd);
}

void Machine::C_NotImplemented()
{
	Abort("Command not implemented");
}

void Machine::C_Nop()
{
	// No operation
}

void Machine::C_Exit()
{
	Running = false;
}

void Machine::C_Halt()
{
	Halted = true;
}

void Machine::C_Push()
{
	for (int i = 0; i < CurrentLine->GetParamCount(); i++)
		ParamStack.push(Parameter(*CurrentLine->NextParam()));
}

void Machine::C_DuplicateStackItem()
{
	ParamStack.push(ParamStack.top());
}

void Machine::C_WindowOpen()
{
	if (Win) {
		Abort("Window already open");
		return;
	}

	int fullscreen = PopNumber();
	int zoom = PopNumber();
	int h = PopNumber();
	int w = PopNumber();

	Win = new TWindow(w, h, zoom, fullscreen);
	Board = new TBoard(Win->Cols, Win->Rows, 1);
	View = new TBoardView(Board, Win, Chars, Pal, 0, 0, Win->Cols, Win->Rows, 255);
}

void Machine::C_WindowClear()
{
	BackColor = PopNumber();
	Win->Clear(Pal, BackColor);
}

void Machine::C_WindowUpdate()
{
	Win->Update();
}

void Machine::C_PaletteLoad()
{
	std::string file = PopString();
	auto lines = TFile::ReadLines(file);
	int index = 0;

	for (auto& line : lines) {
		line = TString::Trim(line);
		if (line.empty())
			continue;

		auto rgb = TString::Split(line, ' ');
		int r = TString::ToInt(rgb[0]);
		int g = TString::ToInt(rgb[1]);
		int b = TString::ToInt(rgb[2]);

		Pal->Set(index, r, g, b);

		index++;
	}
}

void Machine::C_PaletteClear()
{
	Pal->Clear();
}

void Machine::C_PaletteSet()
{
	int b = PopNumber();
	int g = PopNumber();
	int r = PopNumber();
	int index = PopNumber();

	Pal->Set(index, r, g, b);
}

void Machine::C_CharsetLoad()
{
	std::string file = PopString();
	auto lines = TFile::ReadLines(file);
	int data[9] = { 0 };
	int lineIndex = 0;

	for (auto& line : lines) {
		line = TString::Trim(line);
		if (line.empty())
			continue;

		data[lineIndex] = TString::ToInt(line);
		lineIndex++;
		if (lineIndex == 9) {
			Chars->Set(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);
			lineIndex = 0;
		}
	}
}

void Machine::C_CharsetClear()
{
	Chars->Clear();
}

void Machine::C_CharsetSet()
{
	int row7 = PopNumber();
	int row6 = PopNumber();
	int row5 = PopNumber();
	int row4 = PopNumber();
	int row3 = PopNumber();
	int row2 = PopNumber();
	int row1 = PopNumber();
	int row0 = PopNumber();
	int chr = PopNumber();

	Chars->Set(chr, row0, row1, row2, row3, row4, row5, row6, row7);
}

void Machine::C_MapCursorSet()
{
	BoardCursor.Layer = PopNumber();
	BoardCursor.Y = PopNumber();
	BoardCursor.X = PopNumber();
}

void Machine::C_MapTileAdd()
{
	int bgc = PopNumber();
	int fgc = PopNumber();
	int chr = PopNumber();

	TObject* o = Board->GetObjectAt(BoardCursor.X, BoardCursor.Y, BoardCursor.Layer);

	if (o == nullptr) {
		o = new TObject(chr, fgc, bgc);
		Board->PutObject(o, BoardCursor.X, BoardCursor.Y, BoardCursor.Layer);
	}
	else {
		o->AddTile(chr, fgc, bgc);
	}
}

void Machine::C_MapObjectTemplatePut()
{
	ObjectTemplate* temp = ObjTemplates[PopString()];

	TObject* o = Board->GetObjectAt(BoardCursor.X, BoardCursor.Y, BoardCursor.Layer);
	if (o != nullptr)
		Board->DeleteObject(BoardCursor.X, BoardCursor.Y, BoardCursor.Layer);
	
	Board->PutObject(new TObject(*temp->Obj), BoardCursor.X, BoardCursor.Y, BoardCursor.Layer);
}

void Machine::C_ObjectTemplatesLoad()
{
	std::string file = PopString();
	auto lines = TFile::ReadLines(file);
	TTileSequence ts;
	ObjectTemplate* temp = nullptr;
	TObject* o = nullptr;

	for (auto& line : lines) {
		line = TString::Trim(line);
		if (line.empty())
			continue;

		int ixFirstSpace = TString::FindFirst(line, ' ');
		std::string what = TString::ToLower(TString::Trim(line.substr(0, ixFirstSpace)));
		if (what == "end") {
			ObjTemplates[temp->Id] = temp;
			continue;
		}

		std::string data = TString::Trim(line.substr(ixFirstSpace));

		if (what == "id") {
			o = new TObject();
			temp = new ObjectTemplate();
			temp->Id = data;
			temp->Obj = o;
		}
		else if (what == "ts") {
			ts.DeleteAll();
			auto tiles = TString::Split(data, ',');
			for (auto& tile : tiles) {
				auto tileData = TString::Split(tile, ' ');
				ts.Add(TString::ToInt(tileData[0]), TString::ToInt(tileData[1]), TString::ToInt(tileData[2]));
			}
			o->SetTilesEqual(&ts);
		}
		else if (what == "p") {
			ixFirstSpace = TString::FindFirst(data, '=');
			std::string prop = TString::Trim(data.substr(0, ixFirstSpace));
			std::string value = TString::Trim(data.substr(ixFirstSpace + 1));
			o->SetProperty(prop, value);
		}
	}
}

void Machine::C_ObjectTemplatesLoad2()
{
	std::string file = PopString();
	auto lines = TFile::ReadLines(file);
	std::string id = "";
	TTileSequence tileseq;
	bool tileseqParsed = false;
	TObject* o = new TObject();

	for (auto& line : lines) {
		line = TString::Trim(line);
		if (line.empty())
			continue;
		if (id.empty()) {
			id = line;
			continue;
		}
		else if (!tileseqParsed) {
			auto rawTileseq = TString::Split(line, ' ');
			TTile tile;
			int tiledatIndex = 0;
			for (auto& tiledat : rawTileseq) {
				if (tiledatIndex == 0)
					tile.Char = TString::ToInt(tiledat);
				else if (tiledatIndex == 1)
					tile.ForeColor = TString::ToInt(tiledat);
				else if (tiledatIndex == 2)
					tile.BackColor = TString::ToInt(tiledat);

				tiledatIndex++;
				if (tiledatIndex > 2) {
					tiledatIndex = 0;
					tileseq.Add(tile);
				}
			}
			tileseqParsed = true;
			continue;
		}
		
		if (tileseqParsed && TString::ToLower(line) != "end") {
			int ixFirstEqualSign = TString::FindFirst(line, '=');
			std::string prop = TString::Trim(line.substr(0, ixFirstEqualSign));
			std::string value = TString::Trim(line.substr(ixFirstEqualSign));
			o->SetProperty(prop, value);
		}

		if (TString::ToLower(line) == "end") {
			o = new TObject();
			o->SetTilesEqual(&tileseq);
			ObjectTemplate* temp = new ObjectTemplate();
			temp->Id = id;
			temp->Obj = o;
			id = "";
			tileseq.Clear();
			ObjTemplates[temp->Id] = temp;
		}
	}
}

void Machine::C_ObjectTemplateCreate()
{
	std::string id = PopString();
	
	ObjectTemplate* temp = new ObjectTemplate();
	temp->Id = id;
	temp->Obj = new TObject();

	ObjTemplates[id] = temp;
}

void Machine::C_ObjectTemplateTileAdd()
{
	int bgc = PopNumber();
	int fgc = PopNumber();
	int chr = PopNumber();
	std::string id = PopString();

	ObjTemplates[id]->Obj->AddTile(chr, fgc, bgc);
}
