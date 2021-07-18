#include "ProgramEditor.h"

ProgramEditor::ProgramEditor(Graphics* gr, Datafile* data) : EditorBase(gr, data)
{
	AllowClearBuffer = false;
	AllowExitOnEscape = true;
}

ProgramEditor::~ProgramEditor()
{
}

void ProgramEditor::OnStart()
{
}

void ProgramEditor::TypeEnter()
{
	Crlf();
}
