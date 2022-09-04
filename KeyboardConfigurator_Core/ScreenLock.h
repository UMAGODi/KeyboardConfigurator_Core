#pragma once
#pragma comment(lib, "Wtsapi32.lib")

#include <Windows.h>

DWORD WINAPI ScreenLockThreadProc(LPVOID);
static LRESULT CALLBACK DummyWindowProc(HWND, UINT, WPARAM, LPARAM);