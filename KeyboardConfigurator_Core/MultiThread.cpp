#include <Windows.h>
#include "MultiThread.h"

// コンストラクタ
MultiThread::MultiThread(void)
{
  ThreadID = 0;
  hThread = nullptr;
  NoTerminated = FALSE;
}


MultiThread::MultiThread(const LPTHREAD_START_ROUTINE lpStartAdress)
{
  this->MultiThread::MultiThread();
  this->Start(lpStartAdress);
}

MultiThread::MultiThread(const LPTHREAD_START_ROUTINE lpStartAdress, const LPVOID lpParam)
{
  this->MultiThread::MultiThread();
  this->Start(lpStartAdress, lpParam);
}


// デストラクタ
MultiThread::~MultiThread(void)
{
  if (!NoTerminated) this->Terminate(0);
}



// スレッド作成
BOOL MultiThread::Start(const LPTHREAD_START_ROUTINE lpStartAdress)
{
  return this->Start(lpStartAdress, nullptr);
}


BOOL MultiThread::Start(const LPTHREAD_START_ROUTINE lpStartAdress, const LPVOID lpParam)
{
  // スレッドは既に実行中
  if (this->IsAlive()) return FALSE;

  hThread = CreateThread(NULL, 0, lpStartAdress, lpParam, 0, &ThreadID);

  if (hThread == nullptr) return FALSE;

  return TRUE;
}


void MultiThread::LeaveWhenDestruct(const BOOL flag)
{
  NoTerminated = flag;
}


// サスペンド
DWORD MultiThread::Suspend(void)
{
  if (!this->IsAlive()) return (DWORD)-1;

  SuspendThread(hThread);
}


// 再開
DWORD MultiThread::Resume(void)
{
  if (!this->IsAlive()) return (DWORD)-1;

  return ResumeThread(hThread);
}


// 強制終了
void MultiThread::Terminate(const DWORD exitCode)
{
  if (!this->IsAlive()) return;

  TerminateThread(hThread, exitCode);
}


void MultiThread::PostQuit(const DWORD exitCode)
{
  if (!this->IsAlive()) return;

  PostThreadMessage(ThreadID, WM_QUIT, exitCode, 0);
}


DWORD MultiThread::WaitThread(const UINT time)
{
  if (hThread == nullptr) return 0;

  if (this->IsAlive()) WaitForSingleObject(hThread, time);

  DWORD dwExitCode = 0;
  GetExitCodeThread(hThread, &dwExitCode);

  return dwExitCode;
}

// -------------------------------------------------


HANDLE MultiThread::GetThreadHandle(void)
{
  return hThread;
}


DWORD MultiThread::GetThreadID(void)
{
  return ThreadID;
}


BOOL MultiThread::IsAlive(void)
{
  if (hThread == nullptr) return FALSE;

  // スレッド作成済み
  DWORD dwExitCode = 0;

  GetExitCodeThread(hThread, &dwExitCode);

  if (dwExitCode == STILL_ACTIVE) return TRUE;

  return FALSE;
}


// -------------------------------------------------




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ↑MultiThread
// ↓ThreadMaster
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


ThreadMaster::ThreadMaster(void)
{
  locked = FALSE;
}

ThreadMaster::~ThreadMaster(void)
{
  this->TerminateAllThreads();
}


void ThreadMaster::RegisterThread(const std::wstring key, const LPTHREAD_START_ROUTINE proc)
{
  if (locked == FALSE && thread.count(key) == 0) thread[key] = new MultiThread(proc);
}


void ThreadMaster::PostQuitToAllThreads(const int code)
{
  for (auto th : thread)
  {
    th.second->PostQuit(code);
  }
}

void ThreadMaster::SleepUntilThreadsEnd(void)
{
  const DWORD nCount = thread.size();

  if (nCount == 0) return;

  locked = TRUE;

  std::vector<HANDLE> hThreads;

  for (auto th : thread)
  {
    hThreads.emplace_back(th.second->GetThreadHandle());
  }


  WaitForMultipleObjects(nCount, hThreads.data(), TRUE, INFINITE);
}


void ThreadMaster::RemoveEndedThreads(void)
{

  if (thread.size() == 0) return;

  for (auto th : thread)
  {
    // スレッドが作成済み(開始はしている)でスレッドは非アクティブのとき
    if (th.second->GetThreadHandle() != nullptr && th.second->IsAlive() == FALSE)
      thread.erase(th.first);
  }
}


void ThreadMaster::TerminateThread(std::wstring key)
{
  if (thread.count(key) == 0) return;

  delete thread[key];
}

void ThreadMaster::TerminateAllThreads(void)
{
  for (auto th : thread)
  {
    this->TerminateThread(th.first);
  }
}