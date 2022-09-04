#include <Windows.h>
#include <string>
#include <unordered_map>
#include "KeyCode.h"
#include "KeyMap.h"

#define VK2SC(code) 
#define MAKECOMCODE_AUTOSC(vkcode) MAKELONG(MapVirtualKey(vkcode, MAPVK_VK_TO_VSC), vkcode)



void GenerateFullKeyMap(std::unordered_map<std::wstring, DWORD>& maplist)
{
  // ===================================
  // 仮想キーコード順
  // ===================================


  //-------------------
  // Mouse
  //-------------------
  maplist[L"LBUTTON"] = MAKECOMCODE(0, VK_LBUTTON);
  maplist[L"RBUTTON"] = MAKECOMCODE(0, VK_RBUTTON);
  maplist[L"MBUTTON"] = MAKECOMCODE(0, VK_MBUTTON);
  maplist[L"XBUTTON1"] = MAKECOMCODE(0, VK_XBUTTON1);
  maplist[L"XBUTTON2"] = MAKECOMCODE(0, VK_XBUTTON2);

  maplist[L"WHEELUP"] = MAKECOMCODE(0, VK_WHEELUP);
  maplist[L"WHEELDOWN"] = MAKECOMCODE(0, VK_WHEELDOWN);
  maplist[L"HWHEELLEFT"] = MAKECOMCODE(0, VK_HWHEELLEFT);
  maplist[L"HWHEELRIGHT"] = MAKECOMCODE(0, VK_HWHEELRIGHT);


  //-------------------
  // Principal Keys
  //-------------------
  maplist[L"BACK"]    = MAKECOMCODE(8, VK_BACK);
  maplist[L"TAB"]     = MAKECOMCODE(15, VK_TAB);
  maplist[L"CLEAR"]   = MAKECOMCODE(76, VK_CLEAR);
  maplist[L"ENTER"]   = MAKECOMCODE(28, VK_RETURN);
  maplist[L"PAUSE"]   = MAKECOMCODE(69, VK_PAUSE);
  maplist[L"CAPITAL"] = MAKECOMCODE(58, VK_CAPITAL);  // US CapsLock / Japanese Shift+CapsLock 

  //-------------------
  // IME
  maplist[L"KANA"]    = MAKECOMCODE_AUTOSC(VK_KANA);

  maplist[L"JUNJA"]   = MAKECOMCODE_AUTOSC(VK_JUNJA);
  maplist[L"FINAL"]   = MAKECOMCODE_AUTOSC(VK_FINAL);
  maplist[L"KANJI"]   = MAKECOMCODE_AUTOSC(VK_KANJI);

  maplist[L"ESCAPE"] = MAKECOMCODE(1, VK_ESCAPE);

  maplist[L"NONCONVERT"] = MAKECOMCODE(123, VK_NONCONVERT);
  maplist[L"CONVERT"]    = MAKECOMCODE(121, VK_CONVERT);
  //-------------------


  maplist[L"ACCEPT"]     = MAKECOMCODE_AUTOSC(VK_ACCEPT);
  maplist[L"MODECHANGE"] = MAKECOMCODE_AUTOSC(VK_MODECHANGE);

  maplist[L"SPACE"] = MAKECOMCODE(57, VK_SPACE);

  maplist[L"PRIOR"] = MAKECOMCODE(73, VK_PRIOR); // PageUp
  maplist[L"PGUP"] = MAKECOMCODE(73, VK_PRIOR); // PageUp

  maplist[L"NEXT"] = MAKECOMCODE(81, VK_NEXT);  // PageDown
  maplist[L"PGDOWN"] = MAKECOMCODE(81, VK_NEXT);  // PageDown

  maplist[L"END"] = MAKECOMCODE(79, VK_END);
  maplist[L"HOME"] = MAKECOMCODE(71, VK_HOME);


  //-------------------
  // Arrow keys
  maplist[L"LEFT"] = MAKECOMCODE(75, VK_LEFT);
  maplist[L"UP"] = MAKECOMCODE(72, VK_UP);
  maplist[L"RIGHT"] = MAKECOMCODE(77, VK_RIGHT);
  maplist[L"DOWN"] = MAKECOMCODE(80, VK_DOWN);
  //-------------------


  maplist[L"SELECT"] = MAKECOMCODE_AUTOSC(VK_SELECT);
  maplist[L"PRINT"] = MAKECOMCODE_AUTOSC(VK_PRINT);
  maplist[L"EXECUTE"] = MAKECOMCODE_AUTOSC(VK_EXECUTE);

  maplist[L"SNAPSHOT"] = MAKECOMCODE(55, VK_SNAPSHOT);
  maplist[L"PRINTSC"] = MAKECOMCODE(55, VK_SNAPSHOT);


  maplist[L"INSERT"] = MAKECOMCODE(82, VK_INSERT);
  maplist[L"DELETE"] = MAKECOMCODE(83, VK_DELETE);

  maplist[L"HELP"] = MAKECOMCODE_AUTOSC(VK_HELP);


  //-------------------
  // Numbers
  maplist[L"1"] = MAKECOMCODE(2, (WORD)'1');
  maplist[L"2"] = MAKECOMCODE(3, (WORD)'2');
  maplist[L"3"] = MAKECOMCODE(4, (WORD)'3');
  maplist[L"4"] = MAKECOMCODE(5, (WORD)'4');
  maplist[L"5"] = MAKECOMCODE(6, (WORD)'5');
  maplist[L"6"] = MAKECOMCODE(7, (WORD)'6');
  maplist[L"7"] = MAKECOMCODE(8, (WORD)'7');
  maplist[L"8"] = MAKECOMCODE(9, (WORD)'8');
  maplist[L"9"] = MAKECOMCODE(10, (WORD)'9');
  maplist[L"0"] = MAKECOMCODE(11, (WORD)'0');
  //-------------------


  //-------------------
  // Characters
  maplist[L"A"] = MAKECOMCODE(30, (WORD)'A');
  maplist[L"B"] = MAKECOMCODE(48, (WORD)'B');
  maplist[L"C"] = MAKECOMCODE(46, (WORD)'C');
  maplist[L"D"] = MAKECOMCODE(32, (WORD)'D');
  maplist[L"E"] = MAKECOMCODE(18, (WORD)'E');
  maplist[L"F"] = MAKECOMCODE(33, (WORD)'F');
  maplist[L"G"] = MAKECOMCODE(34, (WORD)'G');
  maplist[L"H"] = MAKECOMCODE(35, (WORD)'H');
  maplist[L"I"] = MAKECOMCODE(23, (WORD)'I');
  maplist[L"J"] = MAKECOMCODE(36, (WORD)'J');
  maplist[L"K"] = MAKECOMCODE(37, (WORD)'K');
  maplist[L"L"] = MAKECOMCODE(38, (WORD)'L');
  maplist[L"M"] = MAKECOMCODE(50, (WORD)'M');
  maplist[L"N"] = MAKECOMCODE(49, (WORD)'N');
  maplist[L"O"] = MAKECOMCODE(24, (WORD)'O');
  maplist[L"P"] = MAKECOMCODE(25, (WORD)'P');
  maplist[L"Q"] = MAKECOMCODE(16, (WORD)'Q');
  maplist[L"R"] = MAKECOMCODE(19, (WORD)'R');
  maplist[L"S"] = MAKECOMCODE(31, (WORD)'S');
  maplist[L"T"] = MAKECOMCODE(20, (WORD)'T');
  maplist[L"U"] = MAKECOMCODE(22, (WORD)'U');
  maplist[L"V"] = MAKECOMCODE(47, (WORD)'V');
  maplist[L"W"] = MAKECOMCODE(17, (WORD)'W');
  maplist[L"X"] = MAKECOMCODE(45, (WORD)'X');
  maplist[L"Y"] = MAKECOMCODE(21, (WORD)'Y');
  maplist[L"Z"] = MAKECOMCODE(44, (WORD)'Z');
  //-------------------

  
  maplist[L"LWIN"] = MAKECOMCODE(91, VK_LWIN);
  maplist[L"RWIN"] = MAKECOMCODE(92, VK_RWIN);
  maplist[L"APPS"] = MAKECOMCODE(93, VK_APPS);

  maplist[L"SLEEP"] = MAKECOMCODE_AUTOSC(VK_SLEEP);



  //-------------------
  // Numeric keypad
  maplist[L"NUMPAD0"] = MAKECOMCODE(82, VK_NUMPAD0);
  maplist[L"NUMPAD1"] = MAKECOMCODE(79, VK_NUMPAD1);
  maplist[L"NUMPAD2"] = MAKECOMCODE(80, VK_NUMPAD2);
  maplist[L"NUMPAD3"] = MAKECOMCODE(81, VK_NUMPAD3);
  maplist[L"NUMPAD4"] = MAKECOMCODE(75, VK_NUMPAD4);
  maplist[L"NUMPAD5"] = MAKECOMCODE(76, VK_NUMPAD5);
  maplist[L"NUMPAD6"] = MAKECOMCODE(77, VK_NUMPAD6);
  maplist[L"NUMPAD7"] = MAKECOMCODE(71, VK_NUMPAD7);
  maplist[L"NUMPAD8"] = MAKECOMCODE(72, VK_NUMPAD8);
  maplist[L"NUMPAD9"] = MAKECOMCODE(73, VK_NUMPAD9);

  maplist[L"MULTIPLY"] = MAKECOMCODE(55, VK_MULTIPLY);
  maplist[L"ADD"] = MAKECOMCODE(78, VK_ADD);
  maplist[L"SEPARATOR"] = MAKECOMCODE_AUTOSC(VK_SEPARATOR);
  maplist[L"SUBTRACT"] = MAKECOMCODE(74, VK_SUBTRACT);
  maplist[L"DECIMAL"] = MAKECOMCODE(83, VK_DECIMAL);
  maplist[L"DIVIDE"] = MAKECOMCODE(53, VK_DIVIDE);
  //-------------------


  //-------------------
  // Function Keys
  maplist[L"F1"] = MAKECOMCODE(59, VK_F1);
  maplist[L"F2"] = MAKECOMCODE(60, VK_F2);
  maplist[L"F3"] = MAKECOMCODE(61, VK_F3);
  maplist[L"F4"] = MAKECOMCODE(62, VK_F4);
  maplist[L"F5"] = MAKECOMCODE(63, VK_F5);
  maplist[L"F6"] = MAKECOMCODE(64, VK_F6);
  maplist[L"F7"] = MAKECOMCODE(65, VK_F7);
  maplist[L"F8"] = MAKECOMCODE(66, VK_F8);
  maplist[L"F9"] = MAKECOMCODE(67, VK_F9);
  maplist[L"F10"] = MAKECOMCODE(68, VK_F10);
  maplist[L"F11"] = MAKECOMCODE(87, VK_F11);
  maplist[L"F12"] = MAKECOMCODE(88, VK_F12);

  maplist[L"F13"] = MAKECOMCODE(100, VK_F13);
  maplist[L"F14"] = MAKECOMCODE(101, VK_F14);
  maplist[L"F15"] = MAKECOMCODE(102, VK_F15);
  maplist[L"F16"] = MAKECOMCODE(103, VK_F16);
  maplist[L"F17"] = MAKECOMCODE(104, VK_F17);
  maplist[L"F18"] = MAKECOMCODE(105, VK_F18);
  maplist[L"F19"] = MAKECOMCODE(106, VK_F19);
  maplist[L"F20"] = MAKECOMCODE(107, VK_F20);
  maplist[L"F21"] = MAKECOMCODE(108, VK_F21);
  maplist[L"F22"] = MAKECOMCODE(109, VK_F22);
  maplist[L"F23"] = MAKECOMCODE(110, VK_F23);
  maplist[L"F24"] = MAKECOMCODE(118, VK_F24);
  //-------------------



  maplist[L"NUMLOCK"] = MAKECOMCODE(69, VK_NUMLOCK);
  maplist[L"SCROLL"] = MAKECOMCODE(70, VK_SCROLL);


  //-------------------
  // Modifier keys
  maplist[L"LSHIFT"] = MAKECOMCODE(42, VK_LSHIFT);
  maplist[L"RSHIFT"] = MAKECOMCODE(54, VK_RSHIFT);
  maplist[L"LCONTROL"] = MAKECOMCODE(29, VK_LCONTROL);
  maplist[L"RCONTROL"] = MAKECOMCODE(29, VK_RCONTROL);
  maplist[L"LMENU"] = MAKECOMCODE(56, VK_LMENU);
  maplist[L"RMENU"] = MAKECOMCODE(56, VK_RMENU);
  //-------------------


  //-------------------
  // Browser
  maplist[L"BROWSER_BACK"]      = MAKECOMCODE_AUTOSC(VK_BROWSER_BACK);
  maplist[L"BROWSER_FORWARD"]   = MAKECOMCODE_AUTOSC(VK_BROWSER_FORWARD);
  maplist[L"BROWSER_REFRESH"]   = MAKECOMCODE_AUTOSC(VK_BROWSER_REFRESH);
  maplist[L"BROWSER_STOP"]      = MAKECOMCODE_AUTOSC(VK_BROWSER_STOP);
  maplist[L"BROWSER_SEARCH"]    = MAKECOMCODE_AUTOSC(VK_BROWSER_SEARCH);
  maplist[L"BROWSER_FAVORITES"] = MAKECOMCODE_AUTOSC(VK_BROWSER_FAVORITES);
  maplist[L"BROWSER_HOME"]      = MAKECOMCODE_AUTOSC(VK_BROWSER_HOME);
  //-------------------



  //-------------------
  // Media
  maplist[L"VOLUME_MUTE"] = MAKECOMCODE(0, VK_VOLUME_MUTE);
  maplist[L"VOLUME_DOWN"] = MAKECOMCODE(0, VK_VOLUME_DOWN);
  maplist[L"VOLUME_UP"] = MAKECOMCODE(0, VK_VOLUME_UP);

  maplist[L"MEDIA_NEXT_TRACK"] = MAKECOMCODE(0, VK_MEDIA_NEXT_TRACK);
  maplist[L"MEDIA_PREV_TRACK"] = MAKECOMCODE(0, VK_MEDIA_PREV_TRACK);
  maplist[L"MEDIA_STOP"]       = MAKECOMCODE(0, VK_MEDIA_STOP);
  maplist[L"MEDIA_PLAY_PAUSE"] = MAKECOMCODE(0, VK_MEDIA_PLAY_PAUSE);
  //-------------------

  maplist[L"LAUNCH_MAIL"] = MAKECOMCODE_AUTOSC(VK_LAUNCH_MAIL);
  maplist[L"LAUNCH_MEDIA_SELECT"] = MAKECOMCODE_AUTOSC(VK_LAUNCH_MEDIA_SELECT);
  maplist[L"LAUNCH_APP1"] = MAKECOMCODE_AUTOSC(VK_LAUNCH_APP1);
  maplist[L"LAUNCH_APP2"] = MAKECOMCODE_AUTOSC(VK_LAUNCH_APP2);


  //-------------------
  // Symbol keys
  maplist[L"OEM_1"]      = MAKECOMCODE(40, VK_OEM_1);      // :*
  maplist[L"OEM_PLUS"]   = MAKECOMCODE(39, VK_OEM_PLUS);   // ;+
  maplist[L"OEM_COMMA"]  = MAKECOMCODE(51, VK_OEM_COMMA);  // ,<
  maplist[L"OEM_MINUS"]  = MAKECOMCODE(12, VK_OEM_MINUS);  // -=
  maplist[L"OEM_PERIOD"] = MAKECOMCODE(52, VK_OEM_PERIOD); // .>
  maplist[L"OEM_2"]      = MAKECOMCODE(53, VK_OEM_2);      // /?
  maplist[L"OEM_3"]      = MAKECOMCODE(26, VK_OEM_3);      // @`
  maplist[L"GRAVE"]      = MAKECOMCODE(41, VK_OEM_3);      // US Grave Accent


  maplist[L"OEM_4"] = MAKECOMCODE(27, VK_OEM_4);      // [{
  maplist[L"OEM_5"] = MAKECOMCODE(125, VK_OEM_5);     // \|
  maplist[L"OEM_6"] = MAKECOMCODE(43, VK_OEM_6);      // ]}
  maplist[L"OEM_7"] = MAKECOMCODE(13, VK_OEM_7);      // ^~
  maplist[L"OEM_8"] = MAKECOMCODE_AUTOSC(VK_OEM_8);

  maplist[L"OEM_AX"]   = MAKECOMCODE_AUTOSC(VK_OEM_AX);
  maplist[L"OEM_102"]  = MAKECOMCODE(115, VK_OEM_102);   // \_
  maplist[L"ICO_HELP"] = MAKECOMCODE_AUTOSC(VK_ICO_HELP);
  maplist[L"ICO_00"]   = MAKECOMCODE_AUTOSC(VK_ICO_00);

  maplist[L"PROCESSKEY"] = MAKECOMCODE_AUTOSC(VK_PROCESSKEY);
  maplist[L"ICO_CLEAR"]  = MAKECOMCODE_AUTOSC(VK_ICO_CLEAR);
  maplist[L"PACKET"]     = MAKECOMCODE_AUTOSC(VK_PACKET);

  maplist[L"OEM_RESET"]  = MAKECOMCODE_AUTOSC(VK_OEM_RESET);
  maplist[L"OEM_JUMP"]   = MAKECOMCODE_AUTOSC(VK_OEM_JUMP);
  maplist[L"OEM_PA1"]    = MAKECOMCODE_AUTOSC(VK_OEM_PA1);
  maplist[L"OEM_PA2"]    = MAKECOMCODE_AUTOSC(VK_OEM_PA2);
  maplist[L"OEM_PA3"]    = MAKECOMCODE_AUTOSC(VK_OEM_PA3);
  maplist[L"OEM_WSCTRL"] = MAKECOMCODE_AUTOSC(VK_OEM_WSCTRL);
  maplist[L"OEM_CUSEL"]  = MAKECOMCODE_AUTOSC(VK_OEM_CUSEL);
  
  maplist[L"OEM_ATTN"]    = MAKECOMCODE(58, VK_OEM_ATTN); // Japanese CapsLock
  maplist[L"OEM_FINISH"]  = MAKECOMCODE_AUTOSC(VK_OEM_FINISH);
  maplist[L"OEM_COPY"]    = MAKECOMCODE(112, VK_OEM_COPY); // Katakana/Hiragana

  maplist[L"OEM_AUTO"]    = MAKECOMCODE(41, VK_OEM_AUTO); // Zenkaku/Hankaku ON
  maplist[L"OEM_ENLW"]    = MAKECOMCODE(41, VK_OEM_ENLW); //                 OFF

  maplist[L"OEM_BACKTAB"] = MAKECOMCODE_AUTOSC(VK_OEM_BACKTAB);

  maplist[L"ATTN"]  = MAKECOMCODE_AUTOSC(VK_ATTN);
  maplist[L"CRSEL"] = MAKECOMCODE_AUTOSC(VK_CRSEL);
  maplist[L"EXSEL"] = MAKECOMCODE_AUTOSC(VK_EXSEL);
  maplist[L"EREOF"] = MAKECOMCODE_AUTOSC(VK_EREOF);
  maplist[L"PLAY"]  = MAKECOMCODE_AUTOSC(VK_PLAY);
  maplist[L"ZOOM"]  = MAKECOMCODE_AUTOSC(VK_ZOOM);
  maplist[L"PA1"]   = MAKECOMCODE_AUTOSC(VK_PA1);

  maplist[L"OEM_CLEAR"] = MAKECOMCODE_AUTOSC(VK_OEM_CLEAR);
}


std::wstring SearchKeyNameFromMap(std::unordered_map<std::wstring, DWORD> &maplist, const WORD code)
{
  for (auto mp : maplist)
  {
    if (code == CODE_VKCODE(mp.second))
      return mp.first.data();
  }

  return L"";
}