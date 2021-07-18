#pragma once

#include <TBRLGPT.h>
#include "EditorBase.h"
#include "ProgramEditor.h"
using namespace TBRLGPT;

class CommandShell : public EditorBase
{
public:
	CommandShell(Graphics* gr, Datafile* data);
	virtual ~CommandShell();

private:
	ProgramEditor* ProgEditor;

	void OnStart() override;
	void OnLoop() override;
	void TypeEnter() override;

	void PrintIntro();
	void Ok();
	void InterpretLine(std::string line);
	void PrintCharset(int first, int last);
	void PrintPalette(int first, int last);
	void PrintTemplates();
};
