#pragma once
#include <string>;
#include <filesystem>
#include <vector>
#include <Windows.h>
#include <filesystem>

using std::filesystem::path;

void LoadConfigFile(path p);

static UINT ExtractDoubleQuotedString(std::vector<std::wstring>&, wstring&);