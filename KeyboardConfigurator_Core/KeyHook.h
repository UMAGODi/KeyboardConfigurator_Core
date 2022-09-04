#pragma once
#include <Windows.h>

typedef struct
{
  bool LSHIFT;
  bool RSHIFT;
  bool LCONTROL;
  bool RCONTROL;
  bool LALT;
  bool RALT;
  bool LWIN;
  bool RWIN;

}MODKEYSTATE, *LPMODKEYSTATE;

DWORD WINAPI KeyHookThreadProc(LPVOID);
LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelMouseProc(int, WPARAM, LPARAM);

BYTE RetrievePressedModKeys(void);
void ResetAllInput(void);

static DWORD MouseMsgToVK(const UINT, const DWORD, BOOL&);
static BYTE  GetModBit(const MODKEYSTATE&);
