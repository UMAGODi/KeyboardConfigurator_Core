#pragma once
#include <vector>
#include <Windows.h>
#include <string>
#include <unordered_map>
#include "KbcProfile.h"
#include "MultiThread.h"

extern wstring ExeDir;
extern wstring ExePath;
extern ProfileMaster ProMas;
extern ThreadMaster Thremas;
extern std::unordered_map<std::wstring, DWORD> KeyMapList;