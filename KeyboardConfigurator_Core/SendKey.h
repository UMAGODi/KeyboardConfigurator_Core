#pragma once
#include <Windows.h>
#include <vector>
#include "DisposableThread.h"


enum
{
  TYPE_HEADER,
  TYPE_SENDKEY,
  TYPE_SLEEP
};

struct SENDKEYINFO
{
  WORD wVk;
  WORD wScan;
  BOOL isUp;
};

struct LISTINFO
{
  BOOL needDelete;
};

typedef struct
{
  UINT Type;
  union
  {
    LISTINFO listInfo;
    SENDKEYINFO keyInfo;
    UINT sleep;
  };

}SENDKEYACTION, * LPSENDKEYACTION;


typedef std::vector<SENDKEYACTION> ACTIONLIST, * LPACTIONLIST;

ACTIONLIST& CreateActionList(void);
void AddKeySendAction(ACTIONLIST&, WORD, WORD, BOOL);
void AddKeySendAction(ACTIONLIST&, DWORD, BOOL);
void AddDelayAction(ACTIONLIST&, UINT);
BOOL ExecuteAction(ACTIONLIST&, BOOL);

DWORD WINAPI DispostalKeySendProc(LPVOID);

/*
BOOL SendScanCode(WORD, BOOL);
BOOL SendVKeyCode(WORD, BOOL);
BOOL SendSpecifiedCode(DWORD, BOOL);
BOOL SendSpecifiedCode(WORD, WORD, BOOL);
*/

