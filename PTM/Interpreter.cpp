#include "Interpreter.h"
#include "Util.h"

Interpreter::Interpreter() : 
	Env(NULL), Program(NULL), CurrentLine(NULL), ProgramPtr(0), 
	Running(false), Branching(false), Halted(false), PausedFor(0), ParamPtr(0)
{
}

Interpreter::~Interpreter()
{
}

void Interpreter::Run(class Program* program, Environment* env)
{
	Env = env;
	Env->Cycles = -1;
	Program = program;
	ProgramPtr = 0;
	Running = true;
	Branching = false;
	Halted = false;
	PausedFor = 0;

	SDL_Event e;

	while (Running) {
		SDL_PollEvent(&e);
		if (e.type == SDL_QUIT) {
			Running = false;
			return;
		}
		if (Env->Ui != NULL) {
			Env->DrawEnabledViews();
			Env->Ui->Update();
		}
		if (Halted) {
			continue;
		}
		Env->Cycles++;
		if (Env->Cycles >= ULLONG_MAX) {
			Env->Cycles = 0;
			PausedFor = 0;
		}
		
		if (e.type == SDL_KEYDOWN) {
			Env->LastKeyPressed = e.key.keysym.sym;
		}
		else if (e.type == SDL_KEYUP) {
			Env->LastKeyPressed = 0;
		}

		if (PausedFor > 0) {
			if (Env->Cycles != PausedFor)
				continue;
			else
				PausedFor = 0;
		}

		ProgramLine* line = program->GetLine(ProgramPtr);
		CurrentLine = line;
		Execute(line);

		if (!Running || Halted) {
			continue;
		}
		if (Branching) {
			Branching = false;
		}
		else {
			ProgramPtr++;
			if (ProgramPtr >= program->GetSize()) {
				Abort("Execution pointer past end of program");
			}
		}
	}
}

void Interpreter::Abort(std::string msg)
{
	Running = false;
	ShowErrorMessageBox("PTM - Runtime Error", 
		String::Format("Error at line %i:\n%s\n\n%s", 
		CurrentLine->GetSourceLineNumber(), 
		CurrentLine->GetSourceLine().c_str(),
		msg.c_str()));
}

void Interpreter::ErrorOutOfParams()
{
	Abort("Missing parameters");
}

std::string Interpreter::NextString()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
		return "";
	}

	Parameter* param = CurrentLine->GetParam(ParamPtr);
	ParamPtr++;
	if (param->GetType() == ParameterType::PARAM_STRINGLITERAL ||
		param->GetType() == ParameterType::PARAM_LABEL)
		return param->GetStringValue();
	else if (param->GetType() == ParameterType::PARAM_VARIABLE)
		return Env->GetStringVar(param->GetStringValue());

	return "";
}

int Interpreter::NextNumber()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
		return 0;
	}

	Parameter* param = CurrentLine->GetParam(ParamPtr);
	ParamPtr++;
	if (param->GetType() == ParameterType::PARAM_NUMERIC)
		return param->GetNumericValue();
	else if (param->GetType() == ParameterType::PARAM_VARIABLE)
		return Env->GetNumericVar(param->GetStringValue());

	return 0;
}

std::string Interpreter::NextVariableIdentifier()
{
	if (ParamPtr >= CurrentLine->GetParamCount()) {
		ErrorOutOfParams();
		return "";
	}

	std::string ident = CurrentLine->GetParam(ParamPtr)->GetStringValue();
	ParamPtr++;
	return ident;
}

void Interpreter::Goto()
{
	std::string label = NextString();
	if (Program->HasLabel(label)) {
		ProgramPtr = Program->GetLabel(label);
		Branching = true;
	}
	else {
		Abort("Label not found: " + label);
	}
}

void Interpreter::Call()
{
	std::string label = NextString();
	if (Program->HasLabel(label)) {
		Env->PushToCallStack(ProgramPtr + 1);
		ProgramPtr = Program->GetLabel(label);
		Branching = true;
	}
	else {
		Abort("Label not found: " + label);
	}
}

Object* Interpreter::GetSelectedObject()
{
	if (Env->CurrentMap != NULL)
		return Env->CurrentMap->GetObject(Env->MapCursorX, Env->MapCursorY, Env->MapCursorLayer);

	return NULL;
}

void Interpreter::Execute(ProgramLine* line)
{
	std::string& command = line->GetCommand();
	ParamPtr = 0;

	if (command == "EXIT") {
		Running = false;
	}
	else if (command == "HALT") {
		Halted = true;
	}
	else if (command == "TITLE") {
		Env->WindowTitle = NextString();
		if (Env->Gr != NULL)
			Env->Gr->SetWindowTitle(Env->WindowTitle);
	}
	else if (command == "GOTO") {
		Goto();
	}
	else if (command == "CALL") {
		Call();
	}
	else if (command == "RETURN") {
		ProgramPtr = Env->PopFromCallStack();
		Branching = true;
	}
	else if (command == "PAUSE") {
		PausedFor = Env->Cycles + NextNumber();
	}
	// IFx_GOTO
	else if (command == "IFEQ_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a == b) {
			Goto();
		}
	}
	else if (command == "IFNEQ_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a != b) {
			Goto();
		}
	}
	else if (command == "IFLT_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a < b) {
			Goto();
		}
	}
	else if (command == "IFLE_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a <= b) {
			Goto();
		}
	}
	else if (command == "IFGT_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a > b) {
			Goto();
		}
	}
	else if (command == "IFGE_GOTO") {
		int a = NextNumber();
		int b = NextNumber();
		if (a >= b) {
			Goto();
		}
	}
	// IFx_CALL
	else if (command == "IFEQ_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a == b) {
			Call();
		}
	}
	else if (command == "IFNEQ_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a != b) {
			Call();
		}
	}
	else if (command == "IFLT_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a < b) {
			Call();
		}
	}
	else if (command == "IFLE_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a <= b) {
			Call();
		}
	}
	else if (command == "IFGT_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a > b) {
			Call();
		}
	}
	else if (command == "IFGE_CALL") {
		int a = NextNumber();
		int b = NextNumber();
		if (a >= b) {
			Call();
		}
	}
	else if (command == "MSGBOX") {
		if (line->GetParamCount() == 1) {
			std::string msg = NextString();
			ShowMessageBox("", msg);
		}
		else if (line->GetParamCount() == 2) {
			std::string title = NextString();
			std::string msg = NextString();
			ShowMessageBox(title, msg);
		}
	}
	else if (command == "VAR") {
		std::string var = NextVariableIdentifier();
		Parameter* param2 = line->GetParam(1);
		
		if (param2->GetType() == ParameterType::PARAM_VARIABLE) {
			std::string value = Env->GetStringVar(param2->GetStringValue());
			Env->SetVar(var, value);
		}
		else {
			std::string value = param2->GetStringValue();
			Env->SetVar(var, value);
		}
	}
	else if (command == "INC") {
		std::string var = NextVariableIdentifier();
		Env->SetVar(var, Env->GetNumericVar(var) + 1);
	}
	else if (command == "DEC") {
		std::string var = NextVariableIdentifier();
		Env->SetVar(var, Env->GetNumericVar(var) - 1);
	}
	else if (command == "ADD") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, Env->GetNumericVar(var) + value);
	}
	else if (command == "SUB") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, Env->GetNumericVar(var) - value);
	}
	else if (command == "MUL") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, Env->GetNumericVar(var) * value);
	}
	else if (command == "DIV") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, Env->GetNumericVar(var) / value);
	}
	else if (command == "DIVR") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, Env->GetNumericVar(var) % value);
	}
	else if (command == "POW") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		int value = NextNumber();
		Env->SetVar(result, pow(Env->GetNumericVar(var), value));
	}
	else if (command == "SQRT") {
		std::string result = NextVariableIdentifier();
		std::string var = NextVariableIdentifier();
		Env->SetVar(result, sqrt(Env->GetNumericVar(var)));
	}
	else if (command == "CYCLE") {
		std::string var = NextVariableIdentifier();
		Env->SetVar(var, Env->Cycles);
	}
	else if (command == "RANDOM") {
		std::string var = NextVariableIdentifier();
		int max = NextNumber();
		Env->SetVar(var, Util::Random(max));
	}
	else if (command == "GETK") {
		std::string var = NextVariableIdentifier();
		SDL_Keycode key = NextNumber();
		Env->SetVar(var, Env->LastKeyPressed == key ? 1 : 0);
	}
	else if (command == "SCREEN") {
		if (Env->Gr == NULL) {
			int cols = NextNumber();
			int rows = NextNumber();
			int width = NextNumber();
			int height = NextNumber();
			int fullscreen = NextNumber();
			Env->InitWindow(cols, rows, width, height, fullscreen);
		}
		else {
			Abort("Screen is already created");
		}
	}
	else if (command == "CLS") {
		Env->Ui->Clear();
	}
	else if (command == "LOCATE") {
		Env->GfxCursorX = NextNumber();
		Env->GfxCursorY = NextNumber();
	}
	else if (command == "FCOLOR") {
		Env->Ui->ForeColor = NextNumber();
	}
	else if (command == "BCOLOR") {
		Env->Ui->BackColor = NextNumber();
	}
	else if (command == "COLOR") {
		Env->Ui->ForeColor = NextNumber();
		Env->Ui->BackColor = NextNumber();
	}
	else if (command == "PRINT") {
		Env->Ui->Print(Env->GfxCursorX, Env->GfxCursorY, NextString());
	}
	else if (command == "PUTC") {
		Env->Ui->PutChar(NextNumber(), Env->GfxCursorX, Env->GfxCursorY);
	}
	else if (command == "SETCHR") {
		int ix = NextNumber();
		int row0 = NextNumber();
		int row1 = NextNumber();
		int row2 = NextNumber();
		int row3 = NextNumber();
		int row4 = NextNumber();
		int row5 = NextNumber();
		int row6 = NextNumber();
		int row7 = NextNumber();

		Env->Ui->Chars->SetChar(ix, row0, row1, row2, row3, row4, row5, row6, row7);
	}
	else if (command == "SETPAL") {
		int ix = NextNumber();
		int rgb = NextNumber();

		Env->Ui->Pal->Set(ix, rgb);
	}
	else if (command == "REFRESH") {
		Env->Ui->Update();
	}
	else if (command == "LOAD_PROJ") {
		std::string file = NextString();
		bool ok = Env->LoadProject(file);
		if (!ok) {
			Abort(String::Format("Project file \"%s\" not found", file.c_str()));
		}
	}
	else if (command == "CREATE_MAP") {
		std::string id = NextString();
		if (Env->GetMap(id) == NULL) {
			int width = NextNumber();
			int height = NextNumber();
			int layers = NextNumber();
			Map* map = new Map(Env->Proj, "", width, height, layers);
			map->SetId(id);
			Env->AddMap(map);
		}
		else {
			Abort(String::Format("Map with id \"%s\" already exists", id.c_str()));
		}
	}
	else if (command == "SELECT_MAP") {
		std::string mapid = NextString();
		Map* map = Env->GetMap(mapid);
		if (map != NULL) {
			Env->CurrentMap = map;
		}
		else {
			Abort(String::Format("Map with id \"%s\" does not exist", mapid.c_str()));
		}
	}
	else if (command == "CREATE_VIEW") {
		std::string viewid = NextString();
		std::string mapid = NextString();
		int x = NextNumber();
		int y = NextNumber();
		int width = NextNumber();
		int height = NextNumber();
		int animationDelay = NextNumber();

		Map* map = Env->GetMap(mapid);
		MapViewport* view = new MapViewport(Env->Ui, map, x, y, width, height, 0, 0, animationDelay);
		Env->AddView(viewid, view);
	}
	else if (command == "SET_VIEW_MAP") {
		std::string viewid = NextString();
		std::string mapid = NextString();
		Env->GetView(viewid)->SetMap(Env->GetMap(mapid));
	}
	else if (command == "SCROLL_VIEW_TO") {
		std::string viewid = NextString();
		int scrollX = NextNumber();
		int scrollY = NextNumber();
		Env->GetView(viewid)->SetScroll(scrollX, scrollY);
	}
	else if (command == "SCROLL_VIEW_BY") {
		std::string viewid = NextString();
		int distX = NextNumber();
		int distY = NextNumber();
		Env->GetView(viewid)->ScrollView(distX, distY);
	}
	else if (command == "ENABLE_VIEW") {
		std::string id = NextString();
		Env->EnableView(id, true);
	}
	else if (command == "DISABLE_VIEW") {
		std::string id = NextString();
		Env->EnableView(id, false);
	}
	else if (command == "SELECT_OBJ") {
		if (Env->CurrentMap != NULL) {
			Env->MapCursorX = NextNumber();
			Env->MapCursorY = NextNumber();
			Env->MapCursorLayer = NextNumber();
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_DELETE") {
		if (Env->CurrentMap != NULL) {
			GetSelectedObject()->SetVoid(true);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_ADD_CHAR") {
		if (Env->CurrentMap != NULL) {
			int ix = NextNumber();
			int fgc = NextNumber();
			int bgc = NextNumber();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			o->GetAnimation().AddFrame(ObjectChar(ix, fgc, bgc));
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_SET_CHAR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			int ix = NextNumber();
			int fgc = NextNumber();
			int bgc = NextNumber();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			o->GetAnimation().SetFrame(frame, ObjectChar(ix, fgc, bgc));
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_SET_CHAR_IX") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			int ix = NextNumber();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			ObjectChar& ch = o->GetAnimation().GetFrame(frame);
			ch.Index = ix;
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_SET_CHAR_FCOLOR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			int ix = NextNumber();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			ObjectChar& ch = o->GetAnimation().GetFrame(frame);
			ch.ForeColorIx = ix;
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_SET_CHAR_BCOLOR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			int ix = NextNumber();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			ObjectChar& ch = o->GetAnimation().GetFrame(frame);
			ch.BackColorIx = ix;
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_GET_CHAR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			std::string ix = NextVariableIdentifier();
			std::string fgc = NextVariableIdentifier();
			std::string bgc = NextVariableIdentifier();
			ObjectChar& ch = GetSelectedObject()->GetAnimation().GetFrame(frame);
			Env->SetVar(ix, ch.Index);
			Env->SetVar(fgc, ch.ForeColorIx);
			Env->SetVar(bgc, ch.BackColorIx);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_GET_CHAR_IX") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			std::string ix = NextVariableIdentifier();
			ObjectChar& ch = GetSelectedObject()->GetAnimation().GetFrame(frame);
			Env->SetVar(ix, ch.Index);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_GET_CHAR_FCOLOR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			std::string fgc = NextVariableIdentifier();
			ObjectChar& ch = GetSelectedObject()->GetAnimation().GetFrame(frame);
			Env->SetVar(fgc, ch.ForeColorIx);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_GET_CHAR_BCOLOR") {
		if (Env->CurrentMap != NULL) {
			int frame = NextNumber();
			std::string bgc = NextVariableIdentifier();
			ObjectChar& ch = GetSelectedObject()->GetAnimation().GetFrame(frame);
			Env->SetVar(bgc, ch.BackColorIx);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_SET_PROP") {
		if (Env->CurrentMap != NULL) {
			std::string prop = NextString();
			std::string value = NextString();
			Object* o = GetSelectedObject();
			o->SetVoid(false);
			o->SetProperty(prop, value);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_GET_PROP") {
		if (Env->CurrentMap != NULL) {
			std::string var = NextVariableIdentifier();
			std::string prop = NextString();
			Env->SetVar(var, GetSelectedObject()->GetPropertyAsString(prop));
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_HAS_PROP") {
		if (Env->CurrentMap != NULL) {
			bool has = false;
			std::string var = NextVariableIdentifier();
			if (CurrentLine->GetParamCount() == 3) {
				std::string prop = NextString();
				std::string value = NextString();
				Object* o = GetSelectedObject();
				has = o->HasPropertyValue(prop, value);
			}
			else if (CurrentLine->GetParamCount() == 2) {
				std::string prop = NextString();
				Object* o = GetSelectedObject();
				has = o->HasProperty(prop);
			}
			Env->SetVar(var, has ? 1 : 0);
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_MOVE_BY") {
		if (Env->CurrentMap != NULL) {
			int dx = NextNumber();
			int dy = NextNumber();
			ObjectPosition pos = ObjectPosition(Env->MapCursorX, Env->MapCursorY, Env->MapCursorLayer);
			Object* o = Env->CurrentMap->GetObject(pos.X, pos.Y, pos.Layer);
			if (!o->IsVoid()) {
				Env->CurrentMap->MoveObject(pos, pos.Move(dx, dy));
			}
			else {
				Abort(String::Format("No object found at %i, %i, %i", pos.X, pos.Y, pos.Layer));
			}
		}
		else {
			Abort("No map selected");
		}
	}
	else if (command == "OBJ_MOVE_TO") {
		if (Env->CurrentMap != NULL) {
			int x = NextNumber();
			int y = NextNumber();
			int layer = NextNumber();
			ObjectPosition pos = ObjectPosition(Env->MapCursorX, Env->MapCursorY, Env->MapCursorLayer);
			Object* o = Env->CurrentMap->GetObject(pos.X, pos.Y, pos.Layer);
			if (!o->IsVoid()) {
				Env->CurrentMap->MoveObject(pos, ObjectPosition(x, y, layer));
			}
			else {
				Abort(String::Format("No object found at %i, %i, %i", pos.X, pos.Y, pos.Layer));
			}
		}
		else {
			Abort("No map selected");
		}
	}
	else {
		Abort(String::Format("Invalid command: %s", command.c_str()));
	}
}
