#pragma once
#include <string>

void CreateWorkingDir();
std::string GetWorkingDir();
void PrintFiles(std::string pattern);
void DeleteFile(std::string filename);
void SaveEverything(std::string filename);
void LoadEverything(std::string filename);
