#pragma once

#include <TBRLGPT.h>
#include "EditorBase.h"
using namespace TBRLGPT;

class CommandShell : public EditorBase
{
public:
	CommandShell(Graphics* gr, Datafile* data);
	virtual ~CommandShell();

private:
	void OnStart() override;
	void TypeEnter() override;

	void PrintIntro();
	void Ok();
	void InterpretLine(std::string line);
};
