#pragma once
#include <Windows.h>

struct MODKEYSTATE
{
  bool LSHIFT;
  bool RSHIFT;
  bool LCONTROL;
  bool RCONTROL;
  bool LALT;
  bool RALT;
  bool LWIN;
  bool RWIN;

  BYTE GetModBit(void);

};

typedef MODKEYSTATE* LPMODKEYSTATE;

DWORD WINAPI KeyHookThreadProc(LPVOID);
LRESULT CALLBACK LowLevelKeyboardProc(int, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelMouseProc(int, WPARAM, LPARAM);

BYTE RetrievePressedModKeys(void);
void ResetAllInput(void);

static DWORD MouseMsgToVK(const UINT, const DWORD, BOOL&);
static BYTE  GetModBit(const MODKEYSTATE&);
