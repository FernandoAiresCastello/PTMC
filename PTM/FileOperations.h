#pragma once
#include <string>

void CreateWorkingDir();
std::string GetWorkingDir();
void DeleteFile(std::string filename);
void SaveEverything(std::string filename);
void LoadEverything(std::string filename);
