
#include <Windows.h>
#include "KbcProfile.h"
#include "SendKey.h"
#include "Debug.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// KbcFunction
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

KbcFunction::KbcFunction(void)
{
  this->SetProfType(PTYPE_DISABLE);
}

KbcFunction::KbcFunction(const INT type)
{
  this->SetProfType(type);
}

KbcFunction::~KbcFunction(void)
{

}

INT KbcFunction::GetFuncType(void)
{
  return FuncType;
}



void KbcFunction::SetProfType(const INT type)
{
  FuncType = type;

  switch (type)
  {
  case PTYPE_DISABLE:
    DisableKey.key.Reset();
    break;

  case PTYPE_REMAP:
    RemapKey.before.Reset();
    RemapKey.after.Reset();
    break;

  default:
    FuncType = -1;
    break;
  }
}


BOOL KbcFunction::ProcessKey(const DWORD vkCode, const BOOL isUp)
{
  switch (FuncType)
  {
  case PTYPE_DISABLE:
    if (vkCode == DisableKey.key.VKCode) {
      if (GetKeyState(DisableKey.key.VKCode) < 0) SendSpecifiedCode(DisableKey.key.ComCode(), TRUE);
      return TRUE;
    }
    break;

  case PTYPE_REMAP:
    if (vkCode == RemapKey.before.VKCode)
    {

      if (GetKeyState(RemapKey.before.VKCode) < 0) SendSpecifiedCode(RemapKey.before.ComCode(), TRUE);

      // ====================================
      // マウスキー送信は致命的なバグの解決策が見つからないためパス
      // ====================================
      if ( RemapKey.after.VKCode == VK_LBUTTON    || RemapKey.after.VKCode == VK_RBUTTON || RemapKey.after.VKCode == VK_MBUTTON
        || RemapKey.after.VKCode == VK_XBUTTON1   || RemapKey.after.VKCode == XBUTTON2
        || RemapKey.after.VKCode == VK_WHEELUP    || RemapKey.after.VKCode == VK_WHEELDOWN
        || RemapKey.after.VKCode == VK_HWHEELLEFT || RemapKey.after.VKCode == VK_HWHEELRIGHT)
        return TRUE;

      
      // 押し上げが存在しないキー
      if (vkCode == VK_WHEELUP || vkCode == VK_WHEELDOWN
        || vkCode == VK_HWHEELLEFT || vkCode == VK_HWHEELRIGHT)
      {
        SendSpecifiedCode(RemapKey.after.ComCode(), FALSE);
        Sleep(20);
        SendSpecifiedCode(RemapKey.after.ComCode(), TRUE);
        return TRUE;
      }
      

      return SendSpecifiedCode(RemapKey.after.ComCode(), isUp);
    }
    break;
  }

  return FALSE;
}


void KbcFunction::ResetInputState(void)
{
  WORD vkCode = 0;

  switch (FuncType)
  {
  case PTYPE_DISABLE:
    vkCode = DisableKey.key.VKCode;
    break;

  case PTYPE_REMAP:
    vkCode = RemapKey.before.VKCode;
    break;
  }

  DebugWriteLine(L"vkKey %d is %d", vkCode, (GetKeyState(vkCode) < 0));
  //if (GetKeyState(vkCode) < 0) SendSpecifiedCode(RemapKey.before.VKCode, RemapKey.before.ScCode, TRUE);
  SendSpecifiedCode(RemapKey.before.VKCode, RemapKey.before.ScCode, TRUE);
}
