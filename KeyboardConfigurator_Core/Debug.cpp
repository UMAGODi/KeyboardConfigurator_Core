#ifdef _DEBUG

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <mutex>
#include "Debug.h"

std::mutex dbgmtx;

void InitDebug(void)
{
  DebugWriteLine(TEXT("\n-----------------------------------\n"));
}

void CloseDebug(void)
{

}


void DebugWriteLine(LPCTSTR output, ...)
{
  // 複数スレッド実行するので念のためミューテックス
  std::lock_guard<std::mutex> lock(dbgmtx);

  va_list arg;
  TCHAR buf[512];
  va_start(arg, output);
  vswprintf(buf, sizeof(buf), output, arg);
  va_end(arg);
  swprintf_s(buf, TEXT("%s\n"), buf);
  OutputDebugString(buf);
}


#endif