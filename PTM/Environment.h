#pragma once
#include <map>
#include <stack>
#include <string>
#include <TBRLGPT.h>

using namespace TBRLGPT;

class Environment
{
public:
	std::string WindowTitle = "";
	unsigned long long Cycles = 0;
	Project* Proj = new Project();
	Graphics* Gr = NULL;
	UIContext* Ui = NULL;
	int GfxCursorX = 0;
	int GfxCursorY = 0;

	Environment();
	~Environment();

	void SetVar(std::string var, std::string value);
	void SetVar(std::string var, int value);
	std::string GetStringVar(std::string var);
	int GetNumericVar(std::string var);
	void PushToCallStack(int programPtr);
	int PopFromCallStack();
	void AddMap(std::string& id, Map* map);
	Map* GetMap(std::string& id);
	void AddView(std::string& id, MapViewport* view);
	MapViewport* GetView(std::string& id);

private:
	std::map<std::string, std::string> Vars;
	std::stack<int> CallStack;
	std::map<std::string, Map*> Maps;
	std::map<std::string, MapViewport*> Views;
};
