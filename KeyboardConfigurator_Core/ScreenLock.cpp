#include <Windows.h>
#include <WtsApi32.h>
#include "ScreenLock.h"
#include "Common.h"
#include "GlobalVars.h"
#include "KbcProfile.h"
#include "KeyHook.h"
#include "Debug.h"


static HWND hDummyWindow;

// =============================================
// WinEvent�X���b�h
// =============================================
DWORD WINAPI ScreenLockThreadProc(LPVOID lpParam)
{

  // �g���C�̃��b�Z�[�W�̓E�B���h�E�ɑ����Ă���̂Ń_�~�[�E�B���h�E�쐬
  hDummyWindow = CreateWindow(TEXT("STATIC"), TEXT(""), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
  SetWindowLongPtr(hDummyWindow, GWLP_WNDPROC, (LONG_PTR)DummyWindowProc);

  if (!WTSRegisterSessionNotification(hDummyWindow, NOTIFY_FOR_THIS_SESSION))
  {
    Thremas.PostQuitToAllThreads(0);
    MessageBox(NULL, TEXT("LockScreen�̐ݒ�Ɏ��s���܂����B"), nullptr, MB_OK | MB_ICONSTOP);
    return 0;
  }


  DebugWriteLine(L"ScreenLock is ready to go.");


  // =======================================================
  // ���C�����[�v
  // =======================================================
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // =======================================================
  // �X���b�h�I��
  // =======================================================


  DebugWriteLine(L"WinEventThreadProc has ended.");

  return 0;
}


  static LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
  {

    if (msg == WM_WTSSESSION_CHANGE)
    {
      if (wp == WTS_SESSION_LOCK || wp == WTS_SESSION_UNLOCK)
      {
        ResetAllInput();
      }

      return 1;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
  }