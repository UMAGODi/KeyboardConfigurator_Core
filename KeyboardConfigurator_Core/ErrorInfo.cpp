#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <Windows.h>
#include "GlobalVars.h"
#include "ErrorInfo.h"

/*
std::wstring Errors = L"";

void StackError(const BYTE type, const UINT line, const std::wstring str)
{
  if (Errors != L"")
    Errors += L"\r\n";

  switch (type)
  {
  case ERROR_INFO:
    Errors += L"[i]";
    break;

  case ERROR_WARN:
    Errors += L"[!]";
    break;

  case ERROR_ERROR:
    Errors += L"[X]";
    break;
  }

  Errors += L" (" + std::to_wstring(line) + L"çsñ⁄) " + str;
}

bool ExportError(const std::filesystem::path path)
{
  if (Errors == L"")
    return false;


  std::ofstream file;
  std::wstring name;

  //ñ¢äJî≠
  
}
*/