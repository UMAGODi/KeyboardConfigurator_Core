#include <locale.h>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <Windows.h>
#include "Debug.h"
#include "WinEvent.h"
#include "Common.h"
#include "GlobalVars.h"
#include "KbcProfile.h"


using std::wstring;
using std::filesystem::path;

FOREWINDOWINFO FWindowInfo;

#define CHECK_INTERVAL 500


// =============================================
// WinEvent�X���b�h
// =============================================
DWORD WINAPI WinEventThreadProc(LPVOID lpParam)
{

  HWINEVENTHOOK hWinEvent = nullptr;

  
  hWinEvent = SetWinEventHook(
    EVENT_SYSTEM_FOREGROUND,
    EVENT_SYSTEM_FOREGROUND,
    NULL,
    WinEventProc,
    0,
    0,
    WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS | WINEVENT_SKIPOWNTHREAD);

  // �G���[
  if (hWinEvent == nullptr)
  {
    Thremas.PostQuitToAllThreads(0);
    MessageBox(NULL, TEXT("WinEvent�̐ݒ�Ɏ��s���܂����B"), nullptr, MB_OK | MB_ICONSTOP);
    return 0;
  }
  

  DebugWriteLine(L"WinEvent is ready to go.");


  // =======================================================
  // ���C�����[�v
  // =======================================================
  while (1)
  {
    DWORD dwResult;
    DWORD nCount = 0;

    dwResult = MsgWaitForMultipleObjects(nCount, nullptr, FALSE, CHECK_INTERVAL, QS_ALLPOSTMESSAGE);

    // �G���[
    if (dwResult == WAIT_FAILED)
    {
      Thremas.PostQuitToAllThreads(0);
      break;
    }


    // WinEvent���Ƃ��܂ɘR�ꂪ����̂ŁA�C���^�[�o���ł��Ď�����
    if (dwResult == WAIT_TIMEOUT) {

        // FWindowInfo��hwnd�ƈ���Ă�����ʂ̃A�v���ɐ؂�ւ����Ɣ��f
        if (GetForegroundWindow() != FWindowInfo.Hwnd)
        {
            RetrieveForeWindowInfo(FWindowInfo);
            ProMas.SwitchProfile(FWindowInfo.ExeName, FWindowInfo.Title);
        }

        continue;
    }


    // �Ƃ肠����PeekMessage����ኄ�荞�݂���������炵���B
    // ���炭�B���L���[�Ɋ��荞�݂̓X�^�b�N�����̂ŁAGetMessage���Ɛ��䂪�Ԃ��Ă��Ȃ��ċl��
    if (dwResult == nCount)
    {
      MSG msg;
      PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
      if (msg.message == WM_QUIT) break;
    }
  }

  // =======================================================
  // �X���b�h�I��
  // =======================================================


  DebugWriteLine(L"WinEventThreadProc has ended.");

  UnhookWinEvent(hWinEvent);

  return 1;
}


// =============================================
// WinEvent�v���V�[�W��
// =============================================
void CALLBACK WinEventProc(
  HWINEVENTHOOK hWinEventHook, 
  DWORD event, 
  HWND hwnd, 
  LONG idObject, 
  LONG idChild, 
  DWORD idEventThread, 
  DWORD dwmsEventTime)
{
  if (event == EVENT_SYSTEM_FOREGROUND)
  {
    HWND hwnd = GetForegroundWindow();

    if (hwnd == FWindowInfo.Hwnd)
      return;

    // �t�H�A�O���E���h�E�B���h�E�̏��擾
    RetrieveForeWindowInfo(FWindowInfo);
    // �v���t�@�C���؂�ւ�
    ProMas.SwitchProfile(FWindowInfo.ExeName, FWindowInfo.Title);
  }

}


// =============================================
// �E�B���h�E���擾
// =============================================
static void RetrieveForeWindowInfo(FOREWINDOWINFO &fwinfo)
{
  // hwnd
  HWND hwnd = GetForegroundWindow();
  fwinfo.Hwnd = hwnd;

  // Title
  UINT titlelen = GetWindowTextLengthW(hwnd) + 2;
    
  LPWSTR title = new WCHAR[titlelen];
  GetWindowTextW(hwnd, title, titlelen);

  fwinfo.Title = title;

  // Path
  DWORD pid;
  GetWindowThreadProcessId(hwnd, &pid);

  HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

  if (hProcess)
  {
      // �t�@�C���p�X�擾
      DWORD len = 512;
      fwinfo.FilePath.reserve(len);
      QueryFullProcessImageNameW(hProcess, 0, fwinfo.FilePath.data(), &len);

      CloseHandle(hProcess);

      // Windows�͑啶����������ʂȂ��̂ŏ������œ���
      std::transform(
          fwinfo.FilePath.cbegin(),
          fwinfo.FilePath.cend(),
          fwinfo.FilePath.begin(),
          tolower_);

      // �t�@�C�������ꉞ
      path buf = fwinfo.FilePath.data();
      fwinfo.ExeName = buf.filename().wstring();

  }

  else
  {
    fwinfo.FilePath = TEXT("");
    fwinfo.ExeName = TEXT("");
  }

  // Rect
  GetWindowRect(hwnd, &(fwinfo.Rect));
}