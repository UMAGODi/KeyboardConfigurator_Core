#include <Windows.h>
#include <string>
#include "Common.h"
#include "DataIO.h"
#include "Debug.h"


/*
using std::wstring;

extern DWORD MainThreadID;

static HANDLE hInPipe;
static HANDLE hOutPipe;


// ===================================================
// �p�C�v�ɑ��M
// ===================================================
void PrintToPipe(LPCWSTR output, ...)
{
  va_list arg;
  WCHAR buf[BUFSIZE] = { 0 };

  va_start(arg, output);
  vswprintf(buf, sizeof(buf) / sizeof(WCHAR), output, arg);
  va_end(arg);
  
  DWORD res;
  WriteFile(hOutPipe, buf, sizeof(buf), &res, NULL);
}

// ===================================================
// ���͂��ꂽ�R�}���h������
// ===================================================
BOOL ProcessCommand(wstring &in)
{

  int args;
  LPWSTR* lpArg = CommandLineToArgvW(in.data(), &args);

  if (args <= 0) return TRUE;

  for (int i = 0; i < args; i++)
  {
    LPWSTR cmd = lpArg[i];
  }

  // �R�}���h����
  if (!wcscmp(lpArg[0], L"hello"))
  {
    PrintToPipe(L"hi!");
    return TRUE;
  }

  DebugWriteLine(L"Command coming: %s", in.data());

  PrintToPipe(L"No such command: %s", lpArg[0]);

  return FALSE;
}


// ===================================================
// ���͑ҋ@�X���b�h
// ===================================================
DWORD WINAPI WaitInputThreadProc(LPVOID lpParam)
{

  wstring inMsg;

  inMsg.reserve(BUFSIZE);

  // �p�C�v�쐬
  hInPipe = CreateNamedPipe(
    INPIPENAME,
    PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
    PIPE_TYPE_MESSAGE,
    2,
    inMsg.capacity(), inMsg.capacity(),
    1000,
    NULL);

  hOutPipe = CreateNamedPipe(
    OUTPIPENAME,
    PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
    PIPE_TYPE_MESSAGE,
    2,
    BUFSIZE, BUFSIZE,
    1000,
    NULL);


  // =============
  // �쐬�G���[
  // =============
  if (hInPipe == INVALID_HANDLE_VALUE || hOutPipe == INVALID_HANDLE_VALUE)
  {
    DebugWriteLine(TEXT("Pipe creation failed"));

    if (hInPipe  != INVALID_HANDLE_VALUE) CloseHandle(hInPipe);
    if (hOutPipe != INVALID_HANDLE_VALUE) CloseHandle(hOutPipe);
    
    MessageBox(NULL, TEXT("���O�t���p�C�v�̍쐬�Ɏ��s���܂����B"), nullptr, MB_OK | MB_ICONSTOP);
    return 0;
  }


  // ==========================
  // ���C�����[�v
  // ==========================


  OVERLAPPED ov;
  HANDLE hPipeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  HANDLE hObservedEvent[1];
  DWORD  nCount;

  hObservedEvent[0] = hPipeEvent;
  ov.hEvent = hPipeEvent;

  nCount = sizeof(hObservedEvent) / sizeof(hObservedEvent[0]);

  DebugWriteLine(L"Waiting pipe client... ");


  while(1)
  {
    inMsg.clear();

    ConnectNamedPipe(hInPipe, &ov);
    ConnectNamedPipe(hOutPipe, &ov);

    DWORD dwResult;
    
    // �I�u�W�F�N�g�ҋ@
    dwResult = MsgWaitForMultipleObjects(nCount, &hObservedEvent[0], FALSE, INFINITE, QS_SENDMESSAGE);

    MSG msg;

    // �I����
    if (dwResult == WAIT_OBJECT_0
      || (dwResult == nCount && !GetMessage(&msg, nullptr, 0, 0)))
    {
      DisconnectNamedPipe(hInPipe);
      DisconnectNamedPipe(hOutPipe);
      break;
    }

    
    DWORD res;
    ReadFile(hInPipe, inMsg.data(), inMsg.capacity(), &res, NULL);

    if (!wcscmp(inMsg.data(), L"exit"))
    {
      PrintToPipe(L"Bye :)");
      SetEvent(hExitEvent);
      continue;
    }

    ProcessCommand(inMsg);

    DisconnectNamedPipe(hInPipe);
    DisconnectNamedPipe(hOutPipe);

    break;

  }
  

  // ==========================
  // �I������
  // ==========================

  CloseHandle(hInPipe);
  CloseHandle(hOutPipe);

  SetEvent(hExitEvent);
  
  DebugWriteLine(TEXT("WaitInputThreadProc has ended."));

  return 1;
}

*/