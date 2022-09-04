#pragma once

#include <Windows.h>

DWORD WINAPI TaskTrayThreadProc(LPVOID);
static LRESULT CALLBACK DummyWindowProc(HWND, UINT, WPARAM, LPARAM);
