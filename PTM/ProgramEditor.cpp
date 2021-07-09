#include "ProgramEditor.h"
#include "SharedScreen.h"
#include "Program.h"
#include "TileMachine.h"

struct Program Program;

struct {

	int MaxCommandLength;

	struct {
		int Line;
		int X;
		int Y;
	} Cursor;

	struct {
		int FirstLine;
		int LastLine;
	} View;

	struct {
		int LineNumber;
		int Command;
		int Params;
		int Label;
		int Info;
		int Error;
	} Color;

} PrgEdit;

void InitProgramEditor()
{
	PrgEdit.MaxCommandLength = GetScreenBufferWidth() - 2;
	PrgEdit.Cursor.Line = 0;
	PrgEdit.Cursor.X = 0;
	PrgEdit.Cursor.Y = 0;
	PrgEdit.View.FirstLine = 0;
	PrgEdit.View.LastLine = GetScreenBufferHeight() - 1;

	PrgEdit.Color.LineNumber = 0;
	PrgEdit.Color.Command = 27;
	PrgEdit.Color.Params = 1;
	PrgEdit.Color.Label = 43;
	PrgEdit.Color.Info = 4;
	PrgEdit.Color.Error = 11;

	if (Program.Lines.empty()) {
		AddProgramLine("");
	}
}

void DestroyProgramEditor()
{
	for (int i = 0; i < Program.Lines.size(); i++) {
		ProgramLine* line = Program.Lines[i];
		delete line;
	}
	Program.Lines.clear();
}

void RunProgramEditor()
{
	ClearScreen();

	bool progEditRunning = true;
	while (progEditRunning) {
		DrawScreenBorder();
		DrawScreenBuffer();
		PrintProgramEditor();
		UpdateEntireScreen();

		SDL_Event e = { 0 };
		SDL_PollEvent(&e);

		if (e.type == SDL_QUIT) {
			progEditRunning = false;
			return;
		}
		else if (e.type == SDL_KEYDOWN) {
			SDL_Keycode key = e.key.keysym.sym;
			bool alt = Key::Alt();
			bool ctrl = Key::Ctrl();
			bool shift = Key::Shift();
			if (key == SDLK_ESCAPE) {
				progEditRunning = false;
				return;
			}
			else if (key == SDLK_RETURN && alt) {
				ToggleFullscreen();
			}
			else if (key == SDLK_DOWN) {
				if (ctrl) {
					if (PrgEdit.View.LastLine < Program.Lines.size() - 1) {
						PrgEdit.View.FirstLine++;
						PrgEdit.View.LastLine++;
						PrgEdit.Cursor.Line++;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
				}
				else {
					if (PrgEdit.Cursor.Y < GetScreenBufferHeight() - 1 &&
						PrgEdit.Cursor.Line < Program.Lines.size() - 1) {

						PrgEdit.Cursor.Y++;
						PrgEdit.Cursor.Line++;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
					else {
						if (PrgEdit.View.LastLine < Program.Lines.size() - 1) {
							PrgEdit.View.FirstLine++;
							PrgEdit.View.LastLine++;
							PrgEdit.Cursor.Line++;
							ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
							if (PrgEdit.Cursor.X >= line->Command.length()) {
								PrgEdit.Cursor.X = line->Command.length();
							}
						}
					}
				}
			}
			else if (key == SDLK_UP) {
				if (ctrl) {
					if (PrgEdit.View.FirstLine > 0) {
						PrgEdit.View.FirstLine--;
						PrgEdit.View.LastLine--;
						PrgEdit.Cursor.Line--;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
				}
				else {
					if (PrgEdit.Cursor.Y > 0) {
						PrgEdit.Cursor.Y--;
						PrgEdit.Cursor.Line--;
						ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
						if (PrgEdit.Cursor.X >= line->Command.length()) {
							PrgEdit.Cursor.X = line->Command.length();
						}
					}
					else {
						if (PrgEdit.View.FirstLine > 0) {
							PrgEdit.View.FirstLine--;
							PrgEdit.View.LastLine--;
							PrgEdit.Cursor.Line--;
							ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
							if (PrgEdit.Cursor.X >= line->Command.length()) {
								PrgEdit.Cursor.X = line->Command.length();
							}
						}
					}
				}
			}
			else if (key == SDLK_LEFT) {
				if (PrgEdit.Cursor.X > 0) {
					PrgEdit.Cursor.X--;
				}
			}
			else if (key == SDLK_RIGHT) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X < line->Command.length()) {
					PrgEdit.Cursor.X++;
				}
			}
			else if (key == SDLK_HOME) {
				if (ctrl) {
					PrgEdit.Cursor.X = 0;
					PrgEdit.Cursor.Y = 0;
					PrgEdit.Cursor.Line = 0;
					PrgEdit.View.FirstLine = 0;
					PrgEdit.View.LastLine = GetScreenBufferHeight() - 1;
				}
				else {
					PrgEdit.Cursor.X = 0;
				}
			}
			else if (key == SDLK_END) {
				if (ctrl) {
					PrgEdit.Cursor.Line = Program.Lines.size() - 1;
					ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
					PrgEdit.Cursor.X = line->Command.length();

					if (Program.Lines.size() >= GetScreenBufferHeight()) {
						PrgEdit.Cursor.Y = GetScreenBufferHeight() - 1;
						PrgEdit.View.FirstLine = Program.Lines.size() - GetScreenBufferHeight();
						PrgEdit.View.LastLine = PrgEdit.View.FirstLine + GetScreenBufferHeight() - 1;
					}
					else {
						PrgEdit.Cursor.Y = PrgEdit.Cursor.Line;
					}
				}
				else {
					ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
					PrgEdit.Cursor.X = line->Command.length();
				}
			}
			else if (key == SDLK_BACKSPACE) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X > 0) {
					PrgEdit.Cursor.X--;
					line->Command.erase(line->Command.begin() + PrgEdit.Cursor.X);
				}
			}
			else if (key == SDLK_DELETE) {
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (line->Command.empty()) {
					if (PrgEdit.Cursor.Line < Program.Lines.size() - 1) {
						Program.Lines.erase(Program.Lines.begin() + PrgEdit.Cursor.Line);
						delete line;
						line = NULL;
					}
				}
				else {
					line->Command.erase(line->Command.begin() + PrgEdit.Cursor.X);
				}
			}
			else if (key == SDLK_RETURN) {
				ProgramLine* currentLine = Program.Lines[PrgEdit.Cursor.Line];
				ProgramLine* newline = new ProgramLine();

				if (PrgEdit.Cursor.Y < GetScreenBufferHeight() - 1) {
					PrgEdit.Cursor.Line++;
					PrgEdit.Cursor.Y++;
				}
				else {
					PrgEdit.Cursor.Line++;
					PrgEdit.View.FirstLine++;
					PrgEdit.View.LastLine++;
				}

				if (PrgEdit.Cursor.X == 0) {
					newline->Command = currentLine->Command.substr(PrgEdit.Cursor.X);
					currentLine->Command = "";
				}

				Program.Lines.insert(Program.Lines.begin() + PrgEdit.Cursor.Line, newline);
				PrgEdit.Cursor.X = 0;
				FormatProgramLine(currentLine, true);
			}
			else if (key == SDLK_F5) {
				RunProgram(&Program);
				std::string error = GetTileMachineError();
				if (!error.empty()) {
					PrintProgramEditor();
					DrawScreenBuffer();
					DrawScreenBorder();
					PrintOnBottomLeftBorder(error, PrgEdit.Color.Error);
					UpdateEntireScreen();
					Key::WaitAny();
				}
			}
			else if (IsTypableChar(key)) {
				int ch = (int)key;
				if (shift) {
					ch = String::ShiftChar(ch);
				}
				if (Key::CapsLock() || shift) {
					ch = toupper(ch);
				}
				ProgramLine* line = Program.Lines[PrgEdit.Cursor.Line];
				if (PrgEdit.Cursor.X < line->Command.length()) {
					line->Command[PrgEdit.Cursor.X] = ch;
				}
				else if (line->Command.length() < PrgEdit.MaxCommandLength) {
					line->Command.push_back(ch);
				}
				if (PrgEdit.Cursor.X < line->Command.length()) {
					PrgEdit.Cursor.X++;
				}
				if (ch == SpecialChar::Space) {
					FormatProgramLine(line, false);
				}
			}
		}
	}
}

void AddProgramLine(std::string command)
{
	ProgramLine* line = new ProgramLine();
	line->Command = command;
	Program.Lines.push_back(line);
}

void FormatProgramLine(ProgramLine* line, bool trim)
{
	if (trim) {
		line->Command = String::Trim(line->Command);
	}

	if (String::EndsWith(line->Command, ":")) {
		return;
	}

	int spaceIndex = line->Command.find_first_of(SpecialChar::Space);

	if (spaceIndex > 0) {
		for (int i = 0; i < spaceIndex; i++) {
			line->Command[i] = toupper(line->Command[i]);
		}
	}
	else {
		line->Command = String::ToUpper(line->Command);
	}
}

void PrintProgramEditor()
{
	int row = 0;
	int col1 = 1;

	for (int i = PrgEdit.View.FirstLine; i <= PrgEdit.View.LastLine; i++) {
		ClearLine(row);
		if (i < Program.Lines.size()) {
			ProgramLine* line = Program.Lines[i];
			std::string command = line->Command;

			if (String::EndsWith(String::Trim(command), ":")) {
				Print(command, col1, row, PrgEdit.Color.Label, GetScreenBackColor());
			}
			else {
				int spaceIndex = command.find_first_of(' ');
				if (spaceIndex > 0) {
					std::string name = command.substr(0, spaceIndex);
					std::string params = command.substr(spaceIndex);
					Print(name, col1, row, PrgEdit.Color.Command, GetScreenBackColor());
					Print(params, col1 + name.length(), row, PrgEdit.Color.Params, GetScreenBackColor());
				}
				else {
					Print(command, col1, row, PrgEdit.Color.Command, GetScreenBackColor());
				}
			}
		}
		row++;
	}

	int csrX = PrgEdit.Cursor.X + col1;
	int csrY = PrgEdit.Cursor.Y;

	Object* o = GetScreenBufferObject(csrX, csrY);
	ObjectChar& ch = o->GetAnimation().GetFrame(0);
	int temp = ch.BackColorIx;
	ch.BackColorIx = ch.ForeColorIx;
	ch.ForeColorIx = temp;

	std::string info = String::Format("%i/%i", PrgEdit.Cursor.Line + 1, Program.Lines.size());

	PrintOnBottomLeftBorder("F1:Menu", PrgEdit.Color.Info);
	PrintOnBottomRightBorder(info, PrgEdit.Color.Info);
}

struct Program* GetProgram()
{
	return &Program;
}
