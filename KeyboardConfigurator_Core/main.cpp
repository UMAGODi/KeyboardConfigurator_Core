// KeyboardConfigurator_Core.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <vector>
#include <mutex>

#include "framework.h"
#include "Resource.h"

#include "Common.h"
#include "MultiThread.h"
#include "KbcProfile.h"
#include "Debug.h"

#include "KeyHook.h"
//#include "DataIO.h"
#include "FileIO.h"
#include "WinEvent.h"
#include "TaskIcon.h"
#include "KeyMap.h"
#include "ScreenLock.h"


#define HASHKEY_WINEVENT   L"WinEventThread"
#define HASHKEY_KEYHOOK    L"KeyHookThread"
#define HASHKEY_TASKTRAY   L"TaskTrayThread"
#define HASHKEY_SCREENLOCK L"ScreenLockThread"


using std::vector;

// グローバル変数:
ProfileMaster ProMas;
std::unordered_map<std::wstring, DWORD> KeyMapList;
ThreadMaster Thremas;
wstring ExeDir;
wstring ExePath;


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{


  HANDLE mutex = CreateMutex(NULL, FALSE, MUTEXNAME);
  if (mutex)
  {
    HANDLE mtx = OpenMutex(MUTEX_ALL_ACCESS, FALSE, MUTEXNAME);
    if (mtx == nullptr || WaitForSingleObject(mtx, 500) == WAIT_TIMEOUT)
    {
      MessageBox(NULL, TEXT("KeyboardConfiguratorはすでに起動しています。"), nullptr, MB_OK | MB_ICONSTOP);
      return 0;
    }
  }


  // 各種初期化
  InitDebug();
  DebugWriteLine(TEXT("Start"));
  GenerateFullKeyMap(KeyMapList);


  // ----------------------------------------------------------------------------
  // デフォルトプロファイル作成 (必須)
  
  

#ifdef _DEBUG

  ExePath = L"C:/Users/UMAGODi/Workspace/Visual Studio/KeyboardConfigurator/KeyboardConfigurator_Core/Debug/KeyboardConfigurator_Core.exe";
  ExeDir = L"C:/Users/UMAGODi/Workspace/Visual Studio/KeyboardConfigurator/KeyboardConfigurator_Core/Release";


#else

  WCHAR path[(MAX_PATH + 1) * sizeof(WCHAR)];
  if (GetModuleFileNameW(NULL, path, (MAX_PATH + 1) * sizeof(WCHAR)))
  {
    ExePath = path;
    ExeDir = (wstring)std::filesystem::path(path).remove_filename();
  }


#endif // DEBUG


  // ==================
  // 設定ファイル読み込み
  // ==================
  wstring config = ExeDir;
  config += L"/config.conf";
  LoadConfigFile(config);



  // ----------------------------------------------------------------------------
  // スレッド作成
  Thremas.RegisterThread(HASHKEY_WINEVENT,   WinEventThreadProc);
  Thremas.RegisterThread(HASHKEY_KEYHOOK,    KeyHookThreadProc);
  Thremas.RegisterThread(HASHKEY_TASKTRAY,   TaskTrayThreadProc);
  Thremas.RegisterThread(HASHKEY_SCREENLOCK, ScreenLockThreadProc);



  // ----------------------------------------------------------------------------



  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // メインスレッドは終了まで待機
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  Thremas.SleepUntilThreadsEnd();


  // 各種後処理
  CloseDebug();
  if (mutex) CloseHandle(mutex);

  return 1;
}


