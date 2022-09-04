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
// WinEventスレッド
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

  // エラー
  if (hWinEvent == nullptr)
  {
    Thremas.PostQuitToAllThreads(0);
    MessageBox(NULL, TEXT("WinEventの設定に失敗しました。"), nullptr, MB_OK | MB_ICONSTOP);
    return 0;
  }
  

  DebugWriteLine(L"WinEvent is ready to go.");


  // =======================================================
  // メインループ
  // =======================================================
  while (1)
  {
    DWORD dwResult;
    DWORD nCount = 0;

    dwResult = MsgWaitForMultipleObjects(nCount, nullptr, FALSE, CHECK_INTERVAL, QS_ALLPOSTMESSAGE);

    // エラー
    if (dwResult == WAIT_FAILED)
    {
      Thremas.PostQuitToAllThreads(0);
      break;
    }


    // WinEventだとたまに漏れがあるので、インターバルでも監視する
    if (dwResult == WAIT_TIMEOUT) {

        // FWindowInfoのhwndと違っていたら別のアプリに切り替えたと判断
        if (GetForegroundWindow() != FWindowInfo.Hwnd)
        {
            RetrieveForeWindowInfo(FWindowInfo);
            ProMas.SwitchProfile(FWindowInfo.ExeName, FWindowInfo.Title);
        }

        continue;
    }


    // とりあえずPeekMessageすりゃ割り込みが発生するらしい。
    // 恐らく隠しキューに割り込みはスタックされるので、GetMessageだと制御が返ってこなくて詰む
    if (dwResult == nCount)
    {
      MSG msg;
      PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
      if (msg.message == WM_QUIT) break;
    }
  }

  // =======================================================
  // スレッド終了
  // =======================================================


  DebugWriteLine(L"WinEventThreadProc has ended.");

  UnhookWinEvent(hWinEvent);

  return 1;
}


// =============================================
// WinEventプロシージャ
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

    // フォアグラウンドウィンドウの情報取得
    RetrieveForeWindowInfo(FWindowInfo);
    // プロファイル切り替え
    ProMas.SwitchProfile(FWindowInfo.ExeName, FWindowInfo.Title);
  }

}


// =============================================
// ウィンドウ情報取得
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
      // ファイルパス取得
      DWORD len = 512;
      fwinfo.FilePath.reserve(len);
      QueryFullProcessImageNameW(hProcess, 0, fwinfo.FilePath.data(), &len);

      CloseHandle(hProcess);

      // Windowsは大文字小文字区別ないので小文字で統一
      std::transform(
          fwinfo.FilePath.cbegin(),
          fwinfo.FilePath.cend(),
          fwinfo.FilePath.begin(),
          tolower_);

      // ファイル名も一応
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