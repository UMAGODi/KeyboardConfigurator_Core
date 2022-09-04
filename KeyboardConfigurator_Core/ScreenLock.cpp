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
// WinEventスレッド
// =============================================
DWORD WINAPI ScreenLockThreadProc(LPVOID lpParam)
{

  // トレイのメッセージはウィンドウに送られてくるのでダミーウィンドウ作成
  hDummyWindow = CreateWindow(TEXT("STATIC"), TEXT(""), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
  SetWindowLongPtr(hDummyWindow, GWLP_WNDPROC, (LONG_PTR)DummyWindowProc);

  if (!WTSRegisterSessionNotification(hDummyWindow, NOTIFY_FOR_THIS_SESSION))
  {
    Thremas.PostQuitToAllThreads(0);
    MessageBox(NULL, TEXT("LockScreenの設定に失敗しました。"), nullptr, MB_OK | MB_ICONSTOP);
    return 0;
  }


  DebugWriteLine(L"ScreenLock is ready to go.");


  // =======================================================
  // メインループ
  // =======================================================
  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // =======================================================
  // スレッド終了
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