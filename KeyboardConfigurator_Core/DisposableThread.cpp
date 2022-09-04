#include "DisposableThread.h"



BOOL StartDisposableThread(const LPTHREAD_START_ROUTINE lpStartAdress)
{
  return StartDisposableThread(lpStartAdress, nullptr);
}

BOOL StartDisposableThread(const LPTHREAD_START_ROUTINE lpStartAdress, const LPVOID lpParam)
{
  DWORD ThreadID;
  HANDLE hThread = CreateThread(NULL, 0, lpStartAdress, lpParam, 0, &ThreadID);
  return (hThread != nullptr);
}