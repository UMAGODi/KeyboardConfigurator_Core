#pragma once
#include <Windows.h>
#include <unordered_map>
#include <vector>

class MultiThread
{
private:
  DWORD ThreadID;
  HANDLE hThread;
  BOOL NoTerminated;

  BOOL Start(const LPTHREAD_START_ROUTINE);
  BOOL Start(const LPTHREAD_START_ROUTINE, const LPVOID);

public:
  MultiThread(void);
  MultiThread(const LPTHREAD_START_ROUTINE);
  MultiThread(const LPTHREAD_START_ROUTINE, const LPVOID);
  ~MultiThread(void);

  void LeaveWhenDestruct(const BOOL);

  // Thread function
  DWORD Suspend(void);
  DWORD Resume(void);
  void Terminate(const DWORD);
  void PostQuit(const DWORD);
  DWORD WaitThread(const UINT);

  // Getter
  HANDLE GetThreadHandle(void);
  DWORD  GetThreadID(void);
  BOOL   IsAlive(void);
};



class ThreadMaster
{
private:
  BOOL locked;
  std::unordered_map<std::wstring, MultiThread*> thread;


public:
  ThreadMaster();
  ~ThreadMaster();

  void RegisterThread(const std::wstring, const LPTHREAD_START_ROUTINE);
  void PostQuitToAllThreads(const int);
  void SleepUntilThreadsEnd(void);
  void RemoveEndedThreads(void);
  void TerminateThread(std::wstring);
  void TerminateAllThreads(void);
};