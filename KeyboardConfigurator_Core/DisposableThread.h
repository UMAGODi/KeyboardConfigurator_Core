#pragma once
#include <Windows.h>


BOOL StartDisposableThread(const LPTHREAD_START_ROUTINE);
BOOL StartDisposableThread(const LPTHREAD_START_ROUTINE, const LPVOID);
