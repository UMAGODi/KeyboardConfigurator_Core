#include <Windows.h>
#include <iostream>
#include "KeyHook.h"
#include "Debug.h"
#include "Common.h"
#include "KeyCode.h"
#include "GlobalVars.h"
#include "MultiThread.h"
#include "SendKey.h"
#include "KeyMap.h"



#define GETKEYSTATE(code) bool(GetKeyState(code) < 0)


typedef struct KEYHOOKINFO
{
  int nCode;
  WPARAM wp;
  LPARAM lp;
  DWORD vkCode;

}KEYHOOKINFO, *LPKEYHOOKINFO;


// ���荞�ݐ�ł��g���̂ŃO���[�o��
HHOOK hKeyHook;
HHOOK hMouHook;

MODKEYSTATE ModKeyState = { 0 };


BYTE MODKEYSTATE::GetModBit(void)
{
  BYTE mod = 0;

  mod |= MOD_SHIFT * int(bool(LSHIFT | RSHIFT));
  mod |= MOD_CONTROL * int(bool(LCONTROL | RCONTROL));
  mod |= MOD_ALT * int(bool(LALT | RALT));
  mod |= MOD_WIN * int(bool(LWIN | RWIN));

  return mod;
}



DWORD WINAPI KeyHookThreadProc(LPVOID lpParam)
{

  // ���݂̏C���L�[�̏�Ԃ��i�[
  ModKeyState.LSHIFT = GETKEYSTATE(VK_LSHIFT);
  ModKeyState.RSHIFT = GETKEYSTATE(VK_RSHIFT);

  ModKeyState.LCONTROL = GETKEYSTATE(VK_LCONTROL);
  ModKeyState.RCONTROL = GETKEYSTATE(VK_RCONTROL);

  ModKeyState.LALT = GETKEYSTATE(VK_LMENU);
  ModKeyState.RALT = GETKEYSTATE(VK_RMENU);

  ModKeyState.LWIN = GETKEYSTATE(VK_LWIN);
  ModKeyState.RWIN = GETKEYSTATE(VK_RWIN);


  // �O���[�o���L�[�t�b�N�쐬
  hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, NULL);
  hMouHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, NULL);

  // �t�b�N���s��
  if (hKeyHook == nullptr || hMouHook == nullptr)
  {
    Thremas.PostQuitToAllThreads(0);

    MessageBox(NULL, TEXT("�t�b�N�̐ݒ�Ɏ��s���܂����B"), nullptr, MB_OK | MB_ICONSTOP);

    if (hKeyHook != nullptr) UnhookWindowsHookEx(hKeyHook);
    if (hMouHook != nullptr) UnhookWindowsHookEx(hMouHook);
    hKeyHook = hMouHook = nullptr;

    return 0;
  }

  DebugWriteLine(L"KeyHook is ready to go.");

  // �L�[�t�b�N�͂Ȃ���MsgWaitForMultipleObjects���g���Ȃ�
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    
  }

  UnhookWindowsHookEx(hKeyHook);
  UnhookWindowsHookEx(hMouHook);
  
  DebugWriteLine(L"KeyHookThreadProc has ended.");
  return 1;
}



// ===============================================================
// �L�[�t�b�N�v���V�[�W��
// ===============================================================
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wp, LPARAM lp)
{
  if (nCode < 0) return CallNextHookEx(hKeyHook, nCode, wp, lp);

  KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lp;

  BOOL injected = bool(kb->flags & LLKHF_INJECTED);
  BOOL extended = bool(kb->flags & 1);
  BOOL isUp     = bool(kb->flags & LLKHF_UP);
  WORD vkCode   = (WORD)kb->vkCode;

  // KBC���������L�[����Ȃ����
  if ( injected & extended || !injected)
  {
    //DebugWriteLine(L"key: %s(vk: %d / sc: %d), isUp: %d", SearchKeyNameFromMap(KeyMapList, vkCode).data(), vkCode, kb->scanCode, isUp);


    // ���蓖�Ă�����Ό��͈���Ԃ�
    // Global��茻�݂̃v���t�@�C�����D�悳���
    if (ProMas.Current()->ProcessKey(vkCode, ModKeyState, isUp)) return TRUE;
    if (ProMas.Global()->ProcessKey(vkCode, ModKeyState, isUp)) return TRUE;


    // �C���L�[�𖳌���������ʂ̃L�[�ɂ��Ă����牟����Ă��Ȃ��ɓ����Ȃ̂�
    // ��ɊY�����鏈�����s���A�C���L�[�����������ꂽ�肵�Ȃ���Έȉ��ŉ����ꂽ���肷��

    switch (vkCode)
    {
    case VK_LSHIFT:
      ModKeyState.LSHIFT = !isUp;
      break;

    case VK_RSHIFT:
      ModKeyState.RSHIFT = !isUp;
      break;

    case VK_LCONTROL:
      ModKeyState.LCONTROL = !isUp;
      break;

    case VK_RCONTROL:
      ModKeyState.RCONTROL = !isUp;
      break;

    case VK_LMENU:
      ModKeyState.LALT = !isUp;
      break;

    case VK_RMENU:
      ModKeyState.RALT = !isUp;
      break;

    case VK_LWIN:
      ModKeyState.LWIN = !isUp;
      break;

    case VK_RWIN:
      ModKeyState.RWIN = !isUp;
      break;

    }

    
  }

  return CallNextHookEx(hKeyHook, nCode, wp, lp);
}


// ===============================================================
// �}�E�X�t�b�N�v���V�[�W��
// ===============================================================
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wp, LPARAM lp)
{
  if (nCode < 0) return CallNextHookEx(hMouHook, nCode, wp, lp);

  MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lp;


  if (ms->flags != LLMHF_LOWER_IL_INJECTED && !(wp == WM_MOUSEMOVE))
  {
    BOOL isUp;
    DWORD vkCode = MouseMsgToVK(wp, ms->mouseData, isUp);

    //DebugWriteLine(L"%d", ms->mouseData);

    if (ProMas.Current()->ProcessKey(vkCode, ModKeyState, isUp)) return TRUE;
    if (ProMas.Global()->ProcessKey(vkCode, ModKeyState, isUp)) return TRUE;
  }

  return CallNextHookEx(hMouHook, nCode, wp, lp);
}



// ===============================================================
// ������Ă�C���L�[���擾
// ===============================================================
BYTE RetrievePressedModKeys(void)
{
  BYTE res = 0;

  // Shift
  res |= BYTE(MOD_SHIFT * GETKEYSTATE(VK_LSHIFT));
  res |= BYTE(MOD_SHIFT * GETKEYSTATE(VK_RSHIFT));

  // Control
  res |= BYTE(MOD_CONTROL * GETKEYSTATE(VK_LCONTROL));
  res |= BYTE(MOD_CONTROL * GETKEYSTATE(VK_RCONTROL));

  // Alt
  res |= BYTE(MOD_ALT * GETKEYSTATE(VK_LMENU));
  res |= BYTE(MOD_ALT * GETKEYSTATE(VK_RMENU));

  // Win
  res |= BYTE(MOD_WIN * GETKEYSTATE(VK_LWIN));
  res |= BYTE(MOD_WIN * GETKEYSTATE(VK_RWIN));

  return res;
}


// ===============================================================
// �C���L�[���Z�b�g
// ===============================================================
void ResetAllInput(void)
{
  ModKeyState.LSHIFT    = 0;
  ModKeyState.RSHIFT    = 0;
  ModKeyState.LCONTROL  = 0;
  ModKeyState.RCONTROL  = 0;
  ModKeyState.LALT      = 0;
  ModKeyState.RALT      = 0;
  ModKeyState.LWIN      = 0;
  ModKeyState.RWIN      = 0;

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


static DWORD MouseMsgToVK(const UINT msg, const DWORD mouseData, BOOL& isUp)
{
  switch (msg)
  {
  case WM_LBUTTONDOWN:
    isUp = FALSE;
    return VK_LBUTTON;

  case WM_LBUTTONUP:
    isUp = TRUE;
    return VK_LBUTTON;

  case WM_RBUTTONDOWN:
    isUp = FALSE;
    return VK_RBUTTON;

  case WM_RBUTTONUP:
    isUp = TRUE;
    return VK_RBUTTON;
    
  case WM_MBUTTONDOWN:
    isUp = FALSE;
    return VK_MBUTTON;

  case WM_MBUTTONUP:
    isUp = TRUE;
    return VK_MBUTTON;

  case WM_XBUTTONDOWN:  /*Fall Through*/
  case WM_NCXBUTTONDOWN:
    isUp = FALSE;
    if (HIWORD(mouseData) == XBUTTON1) return VK_XBUTTON1;
    return VK_XBUTTON2;

  case WM_XBUTTONUP:  /*Fall Through*/
  case WM_NCXBUTTONUP:
    isUp = TRUE;
    if (HIWORD(mouseData) == XBUTTON1) return VK_XBUTTON1;
    return VK_XBUTTON2;

  case WM_MOUSEWHEEL:
    isUp = FALSE;
    if ((LONG)mouseData > 0) return VK_WHEELUP;
    return VK_WHEELDOWN;

  case WM_MOUSEHWHEEL:
    isUp = FALSE;
    if ((LONG)mouseData > 0) return VK_HWHEELRIGHT;
    return VK_HWHEELLEFT;

  }

  return 0;
}



static BYTE GetModBit(const MODKEYSTATE& mod)
{
  BYTE buf = 0;

  buf |= MOD_SHIFT * BOOL(mod.LSHIFT || mod.RSHIFT);

  buf |= MOD_CONTROL * BOOL(mod.LCONTROL || mod.RCONTROL);

  buf |= MOD_ALT * BOOL(mod.LALT || mod.RALT);

  buf |= MOD_WIN * BOOL(mod.LWIN || mod.RWIN);

  return buf;
}