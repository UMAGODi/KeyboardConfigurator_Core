
#include <string>
#include <algorithm>
#include <Windows.h>
#include "Common.h"


std::wstring wstrlower(std::wstring str)
{
  std::transform(str.begin(), str.end(), str.begin(), tolower);
  return(str);
}



std::wstring wstrupper(std::wstring str)
{
  std::transform(str.begin(), str.end(), str.begin(), toupper);
  return(str);
}


bool Execute(std::wstring cmd)
{
  STARTUPINFO si = { sizeof(STARTUPINFO) };
  PROCESS_INFORMATION pi;
  BOOL res = CreateProcess(nullptr, cmd.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return res;
}