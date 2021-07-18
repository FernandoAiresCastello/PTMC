#pragma once

#include <TBRLGPT.h>
#include "EditorBase.h"
using namespace TBRLGPT;

class ProgramEditor : public EditorBase
{
public:
	ProgramEditor(Graphics* gr, Datafile* data);
	virtual ~ProgramEditor();

private:
	void OnStart() override;
	void TypeEnter() override;
};
