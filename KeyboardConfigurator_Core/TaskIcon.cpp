#include <Windows.h>
#include "TaskIcon.h"
#include "Debug.h"
#include "Common.h"
#include "GlobalVars.h"
#include "resource.h"

#define ID_TRAYICON (1)
#define WM_TASKTRAY (WM_APP + 1)
#define TIPTEXT TEXT("Kbc_Core")

enum
{
  IDM_OPENFOLDER = 1,
  IDM_RESTART,
  IDM_QUIT
};

static HWND hDummyWindow;
static HMENU hTrayMenu;


DWORD WINAPI TaskTrayThreadProc(LPVOID lpParam)
{

  // トレイのメッセージはウィンドウに送られてくるのでダミーウィンドウ作成
  hDummyWindow = CreateWindow(TEXT("STATIC"), TEXT(""), WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, GetModuleHandle(NULL), nullptr);
  SetWindowLongPtr(hDummyWindow, GWLP_WNDPROC, (LONG_PTR)DummyWindowProc);


  SHFILEINFOW sfi = { 0 };
  SHGetFileInfoW(ExePath.data(), 0, &sfi, sizeof(SHFILEINFOW), SHGFI_ICON);

  NOTIFYICONDATA nid = { 0 };
  nid.cbSize = sizeof(NOTIFYICONDATA);
  nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  nid.hWnd   = hDummyWindow;
  nid.hIcon  = sfi.hIcon;
  nid.uID    = ID_TRAYICON;
  nid.uCallbackMessage = WM_TASKTRAY;
  wcscpy_s(nid.szTip, TIPTEXT);

  Shell_NotifyIcon(NIM_ADD, &nid);

  hTrayMenu = CreatePopupMenu();
  AppendMenu(hTrayMenu, MF_STRING,    IDM_OPENFOLDER, TEXT("フォルダーを開く(&O)"));
  AppendMenu(hTrayMenu, MF_SEPARATOR, 0,              nullptr);
  AppendMenu(hTrayMenu, MF_STRING,    IDM_QUIT,       TEXT("終了(&Q)"));
  AppendMenu(hTrayMenu, MF_STRING,    IDM_RESTART,    TEXT("再起動(&R)"));


  DebugWriteLine(L"TaskTrayThread on going.");


  // =======================================================
  // メインループ
  // =======================================================
  MSG msg;
  while(GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // =======================================================
  // スレッド終了
  // =======================================================

  Shell_NotifyIcon(NIM_DELETE, &nid);

  DebugWriteLine(L"TaskTrayThreadProc has ended.");

  return 1;
}


static LRESULT CALLBACK DummyWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{

  if (msg == WM_TASKTRAY && wp == ID_TRAYICON)
  {
    switch (lp)
    {
    case WM_LBUTTONDOWN:
      Thremas.PostQuitToAllThreads(0);
      break;

    case WM_RBUTTONUP:
    {
      POINT po;
      GetCursorPos(&po);

      SetForegroundWindow(hDummyWindow);
        

      switch (TrackPopupMenu(hTrayMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_RIGHTBUTTON, po.x, po.y, 0, hDummyWindow, nullptr))
      {
      case IDM_OPENFOLDER:
        ShellExecute(NULL, TEXT("open"), ExeDir.data(), NULL, NULL, SW_SHOWNORMAL);
        break;

      case IDM_QUIT:
        Thremas.PostQuitToAllThreads(0);
        break;

      case IDM_RESTART:
        Thremas.PostQuitToAllThreads(0);
        Execute(ExePath);
        break;
      }

      break;
    }

    }
  }

  return DefWindowProc(hwnd, msg, wp, lp);
}