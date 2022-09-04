#pragma once

#include <iostream>
#include <utility>
#include <Windows.h>

#define DEBUGMUTEXNAME TEXT("UMAGODi.KeyboardConfigurator.DebugWriteLine")

#ifdef _DEBUG

void InitDebug(void);
void CloseDebug(void);
void DebugWriteLine(LPCTSTR, ...);

#else

#define InitDebug()
#define CloseDebug()
#define DebugWriteLine(...)

#endif