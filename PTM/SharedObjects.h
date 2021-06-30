#pragma once
#include <string>
#include <vector>
#include <TBRLGPT.h>
using namespace TBRLGPT;

void InitSharedObjects();
void DestroySharedObjects();
Project* GetSharedProject();
void SetSharedDirectory(std::string path);
std::string GetSharedDirectory();
