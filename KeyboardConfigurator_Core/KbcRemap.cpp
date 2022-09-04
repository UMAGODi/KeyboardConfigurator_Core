#include <Windows.h>
#include "KbcProfile.h"
#include "Debug.h"




void KbcFunction::Remap_SetKey(const LONG lBefore, const LONG lAfter)
{
  this->Remap_SetKey(CODE_VKCODE(lBefore), CODE_SCCODE(lBefore), CODE_VKCODE(lAfter), CODE_SCCODE(lAfter));
}


void KbcFunction::Remap_SetKey(const WORD wBeforeVKCode, const WORD wBeforeScCode, const WORD wAfterVKCode, const WORD wAfterScCode)
{
  if (FuncType != PTYPE_REMAP) return;

  RemapKey.before.ScCode = wBeforeScCode;
  RemapKey.before.VKCode = wBeforeVKCode;

  RemapKey.after.ScCode = wAfterScCode;
  RemapKey.after.VKCode = wAfterVKCode;


  DebugWriteLine(
    TEXT("Key remapped! [VKCode: 0x%02X, ScCode: 0x%02X] is now [VKCode: 0x%02X, ScCode: 0x%02X]"),
    wBeforeVKCode, 
    wBeforeScCode,
    wAfterVKCode,
    wAfterScCode);

}


LONG KbcFunction::Remap_GetBeforeComKey(void)
{
  if (FuncType != PTYPE_REMAP) return 0;

  return RemapKey.before.ComCode();
}

LONG KbcFunction::Remap_GetAfterComKey(void)
{
  if (FuncType != PTYPE_REMAP) return 0;

  return RemapKey.after.ComCode();
}

