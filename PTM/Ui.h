#pragma once

#include <TBRLGPT.h>
#include "Datafile.h"
using namespace TBRLGPT;

class Ui
{
public:
	static void DrawWindow(Datafile* data, Graphics* gr, int x, int y, int w, int h, int fgcix, int bgcix);
};
