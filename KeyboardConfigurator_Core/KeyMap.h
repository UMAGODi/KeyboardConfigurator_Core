#pragma once
#include <Windows.h>
#include <string>
#include <unordered_map>

void GenerateFullKeyMap(std::unordered_map<std::wstring, DWORD>&);
std::wstring SearchKeyNameFromMap(std::unordered_map<std::wstring, DWORD>&, const WORD);
