#include <Windows.h>
#include "KbcProfile.h"
#include "Debug.h"

void KbcProfile::Disable_SetKey(const LONG lKey)
{
  this->Disable_SetKey(CODE_SCCODE(lKey), CODE_VKCODE(lKey));
}

void KbcProfile::Disable_SetKey(const WORD wScCode, const WORD wVkCode)
{
  lpDisableKey.emplace_back(new LPDISABLEKEYINFO);

  lpDisableKey.back()->key.ScCode = wScCode;
  lpDisableKey.back()->key.VKCode = wVkCode;

  DebugWriteLine(TEXT("Key disabled! [VKCode: 0x%02X, ScCode: 0x%02X]"), wVkCode, wScCode);
}

