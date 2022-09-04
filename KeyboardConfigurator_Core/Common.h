#pragma once
#include <string>

#define MUTEXNAME TEXT("UMAGODi.KeyboardConfigurator")
#define APPDATALOCALDIRNAME TEXT("KeyboardConfigurator")


#define tolower_ (int (*)(int))tolower

#define limit(num, mn, mx) max(mn, min(num, mx))


std::wstring wstrlower(std::wstring);
std::wstring wstrupper(std::wstring);

bool Execute(std::wstring);