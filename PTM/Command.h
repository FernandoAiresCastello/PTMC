#pragma once
#include <map>
#include <string>
#include "Machine.h"
#include <TileGameLib.h>
using namespace TileGameLib;

namespace Command
{
	extern std::map<byte, void(Machine::*)()> Impl;
	extern std::map<std::string, byte> Name;

	void Init();
}
