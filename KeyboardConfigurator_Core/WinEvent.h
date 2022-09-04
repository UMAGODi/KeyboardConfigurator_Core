#pragma once
#include <Windows.h>
#include <string>

using std::wstring;

struct FOREWINDOWINFO
{
  HWND Hwnd;
  wstring Title;
  wstring FilePath;
  wstring ExeName;
  RECT Rect;
};

DWORD WINAPI WinEventThreadProc(LPVOID);
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
static void RetrieveForeWindowInfo(FOREWINDOWINFO&);