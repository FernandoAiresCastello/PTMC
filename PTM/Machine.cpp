#include <Windows.h>
#include "Machine.h"

void Machine::InitCommandMap()
{
	// ===[ SPECIAL ]===============================================================
	CMD("NOP", C_Nop);
	// ===[ DEBUGGING ]=============================================================
	CMD("BREAK", C_Breakpoint);
	// ===[ EXECUTION FLOW ]========================================================
	CMD("EXIT", C_Exit);
	CMD("HALT", C_Halt);
	// ===[ STACK ]=================================================================
	CMD("PUSH", C_Push);
	CMD("DUP", C_DuplicateStackItem);
	CMD("POP", C_Pop);
	// ===[ WINDOW ]================================================================
	CMD("WND.OPEN", C_WindowOpen);
	CMD("WND.CLEAR", C_WindowClear);
	CMD("WND.UPDATE", C_WindowUpdate);
	// ===[ PALETTE ]===============================================================
	CMD("PAL.LOAD", C_PaletteLoad);
	CMD("PAL.CLEAR", C_PaletteClear);
	CMD("PAL.SET", C_PaletteSet);
	// ===[ CHARSET ]================================================================
	CMD("CHR.LOAD", C_CharsetLoad);
	CMD("CHR.CLEAR", C_CharsetClear);
	CMD("CHR.SET", C_CharsetSet);
	// ===[ OBJECT TEMPLATES ]=======================================================
	CMD("OBJT.LOAD", C_ObjectTemplatesLoad);
	CMD("OBJT.NEW", C_ObjectTemplateCreate);
	CMD("OBJT.TILE.ADD", C_ObjectTemplateTileAdd);
	// ===[ MAP VIEW ]===============================================================
	CMD("MAP.VIEW.NEW", C_MapViewCreate);
	CMD("MAP.VIEW.SHOW", C_MapViewEnable);
	CMD("MAP.VIEW.HIDE", C_MapViewDisable);
	// ===[ MAP ]====================================================================
	CMD("MAP.NEW", C_MapCreate);
	CMD("MAP.SELECT", C_MapSelect);
	CMD("MAP.LOAD", C_MapLoad);
	CMD("MAP.CURSOR.SET", C_MapCursorSet);
	CMD("MAP.TILE.ADD", C_MapTileAdd);
	CMD("MAP.OBJT.PUT", C_MapObjectTemplatePut);
	CMD("MAP.STRING.PUT", C_MapPutObjectString);
}

Machine::Machine()
{
	Running = false;
	Halted = false;
	CurrentLine = nullptr;
	Win = nullptr;
	Pal = TPalette::Default;
	Chars = TCharset::Default;
	BackColor = 0;
	SelectedMap = nullptr;
	MapCursor.X = 0;
	MapCursor.Y = 0;
	MapCursor.Layer = 0;

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

	for (auto& view : Views) {
		delete view.second;
		view.second = nullptr;
	}
	Views.clear();

	for (auto& map : Maps) {
		delete map.second;
		map.second = nullptr;
	}
	Maps.clear();

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
		if (Win) {
			Win->Clear(Pal, BackColor);
			for (auto& view : Views) {
				if (view.second->GetBoard() != nullptr)
					view.second->Draw();
			}
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
			else if (e.key.keysym.sym == SDLK_PAUSE) {
				SDL_TriggerBreakpoint();
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
		Abort("Empty parameter stack");
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

// ===[ COMMAND IMPLEMENTATION ]===============================================

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

void Machine::C_Breakpoint()
{
	SDL_TriggerBreakpoint();
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

void Machine::C_Pop()
{
	Pop();
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

		Pal->Set(index, TString::ToInt(line));
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

void Machine::C_MapCreate()
{
	int layers = PopNumber();
	int height = PopNumber();
	int width = PopNumber();
	std::string id = PopString();
	
	if (Maps.find(id) == Maps.end())
		Maps[id] = new TBoard(width, height, layers);
	else
		Abort("Duplicated map id: " + id);
}

void Machine::C_MapSelect()
{
	std::string id = PopString();
	if (Maps.find(id) != Maps.end())
		SelectedMap = Maps[id];
	else
		Abort("Map not found with id: " + id);
}

void Machine::C_MapViewCreate()
{
	int animDelay = PopNumber();
	int height = PopNumber();
	int width = PopNumber();
	int y = PopNumber();
	int x = PopNumber();
	std::string id = PopString();

	if (Views.find(id) == Views.end())
		if (Maps[id] != nullptr) {
			Views[id] = new TBoardView(Maps[id], Win, Chars, Pal, x, y, width, height, animDelay);
			Views[id]->SetEnabled(false);
		}
		else {
			Abort("Referenced map not found: " + id);
		}
	else {
		Abort("Duplicated view id: " + id);
	}
}

void Machine::C_MapViewEnable()
{
	std::string id = PopString();
	Views[id]->SetEnabled(true);
}

void Machine::C_MapViewDisable()
{
	std::string id = PopString();
	Views[id]->SetEnabled(false);
}

void Machine::C_MapLoad()
{
	std::string file = PopString();
	std::string id = PopString();

	auto lines = TFile::ReadLines(file);
	std::string name = TString::Trim(lines[0]);
	std::string sizeStr = TString::Trim(lines[1]);
	auto size = TString::Split(sizeStr, ' ');
	int width = TString::ToInt(TString::Trim(size[0]));
	int height = TString::ToInt(TString::Trim(size[1]));
	int layers = TString::ToInt(TString::Trim(size[2]));

	TBoard* map = new TBoard(width, height, layers);
	if (Maps.find(id) != Maps.end()) {
		delete Maps[id];
		Maps.erase(id);
	}
	Maps[id] = map;

	int lineIndex = 2;
	std::string line = "";
	while (line.empty()) {
		line = TString::Trim(lines[lineIndex]);
		lineIndex++;
		if (lineIndex >= lines.size())
			break;
	}
	lineIndex--;

	int layer = 0;
	int x = 0;
	int y = 0;

	for (lineIndex; lineIndex < lines.size(); lineIndex++) {
		line = TString::Trim(lines[lineIndex]);
		if (line.empty())
			continue;

		auto data = TString::Split(line, ' ');
		if (TString::ToLower(data[0]) == "end") {
			layer++;
			x = 0;
			y = 0;
			continue;
		}

		int count = TString::ToInt(TString::Trim(data[0]));
		std::string tempId = TString::Trim(data[1]);

		for (int i = 0; i < count; i++) {
			if (tempId == "<null>") {
				map->DeleteObject(x, y, layer);
			}
			else {
				TObject* o = new TObject(*ObjTemplates[tempId]->Obj);
				map->PutObject(o, x, y, layer);
			}

			x++;
			if (x >= width) {
				x = 0;
				y++;
			}
		}
	}
}

void Machine::C_MapCursorSet()
{
	MapCursor.Layer = PopNumber();
	MapCursor.Y = PopNumber();
	MapCursor.X = PopNumber();
}

void Machine::C_MapTileAdd()
{
	int bgc = PopNumber();
	int fgc = PopNumber();
	int chr = PopNumber();

	TObject* o = SelectedMap->GetObjectAt(MapCursor.X, MapCursor.Y, MapCursor.Layer);

	if (o == nullptr) {
		o = new TObject(chr, fgc, bgc);
		SelectedMap->PutObject(o, MapCursor.X, MapCursor.Y, MapCursor.Layer);
	}
	else {
		o->AddTile(chr, fgc, bgc);
	}
}

void Machine::C_MapObjectTemplatePut()
{
	ObjectTemplate* temp = ObjTemplates[PopString()];

	TObject* o = SelectedMap->GetObjectAt(MapCursor.X, MapCursor.Y, MapCursor.Layer);
	if (o != nullptr)
		SelectedMap->DeleteObject(MapCursor.X, MapCursor.Y, MapCursor.Layer);
	
	SelectedMap->PutObject(new TObject(*temp->Obj), MapCursor.X, MapCursor.Y, MapCursor.Layer);
}

void Machine::C_MapPutObjectString()
{
	std::string str = PopString();
	int bgc = PopNumber();
	int fgc = PopNumber();
	int layer = PopNumber();
	int y = PopNumber();
	int x = PopNumber();

	for (auto& ch : str) {
		TObject* o = new TObject(ch, fgc, bgc);
		SelectedMap->PutObject(o, x, y, layer);
		x++;
	}
}
