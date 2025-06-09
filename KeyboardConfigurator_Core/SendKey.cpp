#include <Windows.h>
#include <mutex>
#include "SendKey.h"
#include "KeyCode.h"
#include "Debug.h"


ACTIONLIST& CreateActionList(void)
{
  ACTIONLIST& refAction = *(new ACTIONLIST);

  refAction.emplace_back();
  refAction.front().Type = TYPE_HEADER;

  return refAction;
}


void AddKeySendAction(ACTIONLIST& refList, DWORD lCode, BOOL isUp)
{
  AddKeySendAction(refList, CODE_VKCODE(lCode), CODE_SCCODE(lCode), isUp);
}

void AddKeySendAction(ACTIONLIST& refList, WORD wVk, WORD wScan, BOOL isUp)
{
  refList.emplace_back();

  refList.back().Type = TYPE_SENDKEY;
  refList.back().keyInfo.wVk = wVk;
  refList.back().keyInfo.wScan = wScan;
  refList.back().keyInfo.isUp  = isUp;
}


void AddDelayAction(ACTIONLIST& refList, UINT time)
{
  refList.emplace_back();

  refList.back().Type  = TYPE_SLEEP;
  refList.back().sleep = time;
}



BOOL ExecuteAction(ACTIONLIST& refList, BOOL deleteStruct)
{
  if (refList.size() <= 1) // 要素１はヘッダ情報なのでほんへは２以降
    return FALSE;

  refList.front().listInfo.needDelete = deleteStruct;

  return StartDisposableThread(DispostalKeySendProc, &refList);
}



DWORD WINAPI DispostalKeySendProc(LPVOID lpParam)
{

  // 参照使ってるからヌルポは無視
  ACTIONLIST& refList = *(LPACTIONLIST)lpParam;

  if (refList.size() == 0)
    return 0;



  static std::mutex mtx;
  mtx.lock();

  if (refList.size() >= 2) // 要素１はヘッダ情報なので本体は２以降
  {
    for (auto i : refList)
    {
      // HEADER
      if (i.Type == TYPE_HEADER)
        continue;


      // SLEEP
      if (i.Type == TYPE_SLEEP)
      {
        Sleep(i.sleep);
        continue;
      }


      // SENDKEY
      if (i.Type == TYPE_SENDKEY)
      {
        WORD wVk   = i.keyInfo.wVk;
        WORD wScan = i.keyInfo.wScan;
        BOOL isUp  = i.keyInfo.isUp;


        INPUT input = { 0 };


        // マウスボタン
        if (wVk == VK_LBUTTON || wVk == VK_RBUTTON || wVk == VK_MBUTTON
          || wVk == VK_XBUTTON1 || wVk == VK_XBUTTON2
          || wVk == VK_WHEELUP || wVk == VK_WHEELDOWN
          || wVk == VK_HWHEELLEFT || wVk == VK_HWHEELRIGHT)
        {

          input.type = INPUT_MOUSE;
          input.mi.dwFlags = MOUSEEVENTF_MOVE_NOCOALESCE;


          switch (wVk)
          {
          case VK_LBUTTON:
            if (isUp) input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
            else      input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
            break;

          case VK_RBUTTON:
            if (isUp) input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
            else      input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
            break;

          case VK_MBUTTON:
            if (isUp) input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
            else      input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
            break;

          case VK_XBUTTON1:
            input.mi.mouseData = XBUTTON1;
            if (isUp) input.mi.dwFlags |= MOUSEEVENTF_XUP;
            else      input.mi.dwFlags |= MOUSEEVENTF_XDOWN;
            break;

          case VK_XBUTTON2:
            input.mi.mouseData = XBUTTON2;
            if (isUp) input.mi.dwFlags |= MOUSEEVENTF_XUP;
            else      input.mi.dwFlags |= MOUSEEVENTF_XDOWN;
            break;

          case VK_WHEELUP:
            input.mi.mouseData = WHEEL_DELTA;
            input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
            break;

          case VK_WHEELDOWN:
            input.mi.mouseData = -WHEEL_DELTA;
            input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
            break;

          case VK_HWHEELLEFT:
            input.mi.mouseData = -WHEEL_DELTA;
            input.mi.dwFlags |= MOUSEEVENTF_HWHEEL;
            break;

          case VK_HWHEELRIGHT:
            input.mi.mouseData = WHEEL_DELTA;
            input.mi.dwFlags |= MOUSEEVENTF_HWHEEL;
            break;
          }


        }

        // キーボード
        else
        {
          input.type = INPUT_KEYBOARD;
          input.ki.wVk = wVk;
          input.ki.wScan = wScan;
          input.ki.dwFlags = ((UINT)isUp * KEYEVENTF_KEYUP);
          input.ki.time = 0;
          input.ki.dwExtraInfo = 0;
        }

        SendInput(1, &input, sizeof(INPUT));
        DebugWriteLine(L"KeySend %d, %d", wVk, isUp);
      }
    }
  }


  // ################################################################################
  // ヘッダの needDelete が TRUE の場合ここ以外で delete されないので
  // 絶対にdeleteすること！！！！！！！！！！！！！！！！！！！！！！！！
  // ################################################################################
  if (refList.front().listInfo.needDelete == TRUE)
  {
    delete &refList;
    DebugWriteLine(L"lpList has been deleted");
  }


  // 絶対
  mtx.unlock();


  return 0;
}







// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%





