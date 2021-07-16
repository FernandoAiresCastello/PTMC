#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

#define APPLICATION_NAME "PTM 1.0"

void InitSharedObjects();
void DestroySharedObjects();
Project* GetSharedProject();
Charset* GetSharedCharset();
Palette* GetSharedPalette();
