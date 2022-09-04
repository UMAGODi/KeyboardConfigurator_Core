#include <fstream>
#include <iostream>
#include <string>
#include <codecvt>
#include <regex>
#include "Debug.h"
#include "KbcProfile.h"
#include "regexp.h"
#include "ErrorInfo.h"
#include "FileIO.h"
#include "GlobalVars.h"
#include "Common.h"
#include "includes/javacommons/strconv.h"

using std::ifstream;
using std::string;
using std::wstring;


#define PROFNAME_DEFAULT L"default"
#define PROFNAME_GLOBAL  L"global"





void LoadConfigFile(path p)
{
  ifstream ifs(p.wstring());

  if (!ifs.is_open()) return;


  string str;


  // �p�^�[����`
  static const wstring SPACE     = L"[ \\t]*";
  static const wstring KEY       = L"(?:([\\+\\^!#\\$]*)(\\w+))";
  static const wstring RUN       = L"run\\(" + SPACE + L"\"(.+)\"" + SPACE + L"\\)";
  static const wstring DISABLE   = L"(?:disable|0)";
  static const wstring ENDOFLINE = L"[ \\t]*(?:\\/\\/[\\w\\W]*)?$";

  static const wstring EXE       = L"@exe\\(" + SPACE + L"\"(.+)\"" + SPACE + L"\\)";
  static const wstring TITLE     = L"@title\\(" + SPACE + L"\"(.+)\"" + SPACE + L"\\)";
  static const wstring IMPORT    = L"@import\\(" + SPACE + L"\"(.+)\"" + SPACE + L"\\)";


  KbcProfile* lpProf = nullptr;
  UINT line = 0;

  // ===========================
  // 1�s���ǂݍ���
  // ===========================
  while (getline(ifs, str))
  {
    wstring str2 = utf8_to_wide(str);
    wstring buf;

    line++;

    // ======================
    // �R�����g�s
    // ======================
    buf = matchw(str2, L"^" + SPACE + L"\\/\\/", true);
    if (buf != L"")
      continue;


    // ======================
    // �v���t�@�C����`
    // ======================
    buf = submatchw(str2, L"^" + SPACE + L"<(.+)>" + SPACE + L"$", true);
    if (buf != L"")
    {
      // �f�t�H���g�v���t�@�C��
      if (wstrlower(buf) == PROFNAME_DEFAULT)
      {
        lpProf = ProMas.Default();
      }

      // �O���[�o���v���t�@�C��
      else if (wstrlower(buf) == PROFNAME_GLOBAL)
      {
        lpProf = ProMas.Global();
      }

      // ���̑��̃v���t�@�C���i�V�K�쐬�j
      else
      {
        lpProf = ProMas.AddProfile(buf);
      }

      continue;
    }

    // -------------------------------------------------------------------------
    // �Ȍ�̓v���t�@�C���ɒǉ�����ݒ�Ȃ̂ŁA�v���t�@�C�����I�����Ă��Ȃ���΃X�L�b�v����
    // -------------------------------------------------------------------------
    if (lpProf == nullptr) continue;


    vector<wstring> mt;

    
    // ======================
    // @exe
    // ======================
    buf = submatchw(str2, L"^" + SPACE + L"(" + EXE + L"(?:" + SPACE + L"," + SPACE + EXE + L")*)" + ENDOFLINE, true);
    if (buf != L"")
    {
      vector<wstring> list;

      if (ExtractDoubleQuotedString(list, buf))
      {
        for (auto i : list)
        {
          lpProf->AddExeAssign(i);
        }
      }
      continue;
    }
      

    // ======================
    // @title
    // ======================
    buf = submatchw(str2, L"^" + SPACE + L"(" + TITLE + L"(?:" + SPACE + L"," + SPACE + TITLE + L")*)" + ENDOFLINE, true);
    if (buf != L"")
    {
      vector<wstring> list;

      if (ExtractDoubleQuotedString(list, buf))
      {
        for (auto i : list)
        {
          lpProf->AddTitleAssign(i);
        }
      }
      continue;
    } 


    // ======================
    // @import
    // ======================
    buf = submatchw(str2, L"^" + SPACE + L"(" + IMPORT + L"(?:" + SPACE + L"," + SPACE + IMPORT + L")*)" + ENDOFLINE, true);
    if (buf != L"")
    {
      vector<wstring> list;

      if (ExtractDoubleQuotedString(list, buf))
      {
        for (auto i : list)
        {
          ProMas.CopyProfile(i, lpProf->GetName());
        }
      }
      continue;
    }


    // ======================
    // ��������`
    // ======================
    if (matchesw(mt, str2, L"^" + SPACE + KEY + SPACE + L"-->" + SPACE + DISABLE + ENDOFLINE, true))
    {
      wstring keyname = wstrupper(mt[2]);

      // �Y������L�[��������Ȃ�����
      if (KeyMapList.count(keyname) == 0)
        continue;

      WORD code = CODE_VKCODE(KeyMapList[keyname]);
      BYTE mod = 0;

      // Shift
      if (mt[1].find_first_of(L'+') != wstring::npos)
        mod |= MOD_SHIFT;

      // Control
      if (mt[1].find_first_of(L'^') != wstring::npos)
        mod |= MOD_CONTROL;

      // Alt
      if (mt[1].find_first_of(L'!') != wstring::npos)
        mod |= MOD_ALT;

      // Win
      if (mt[1].find_first_of(L'#') != wstring::npos)
        mod |= MOD_WIN;

      // Solo
      if (mt[1].find_first_of(L'$') != wstring::npos)
        mod = MOD_SOLO;



      lpProf->AddDisableKey(code, mod);

      DebugWriteLine(L"%s(%d) with %d at %s has been disabled", keyname.data(), code, mod, lpProf->GetName().data());

      continue;
    }
    

    // ======================
    // ���}�b�v
    // ======================
    if (matchesw(mt, str2, L"^" + SPACE + KEY + SPACE + L"-->" + SPACE + KEY + ENDOFLINE, true))
    {
      wstring keyname      = wstrupper(mt[2]);
      wstring afterkeyname = wstrupper(mt[4]);

      // �Y������L�[��������Ȃ�����
      if (KeyMapList.count(keyname) == 0 || KeyMapList.count(afterkeyname) == 0)
        continue;

      WORD code      = CODE_VKCODE(KeyMapList[keyname]);
      DWORD aftercode = KeyMapList[afterkeyname];
      BYTE mod = 0;
      BYTE aftermod = 0;

      // -----------------------------------------------
      // Shift
      if (mt[1].find_first_of(L'+') != wstring::npos)
        mod |= MOD_SHIFT;

      // Control
      if (mt[1].find_first_of(L'^') != wstring::npos)
        mod |= MOD_CONTROL;

      // Alt
      if (mt[1].find_first_of(L'!') != wstring::npos)
        mod |= MOD_ALT;

      // Win
      if (mt[1].find_first_of(L'#') != wstring::npos)
        mod |= MOD_WIN;

      // Solo
      if (mt[1].find_first_of(L'$') != wstring::npos)
        mod = MOD_SOLO;


      // -----------------------------------------------
      // Shift
      if (mt[3].find_first_of(L'+') != wstring::npos)
        aftermod |= MOD_SHIFT;

      // Control
      if (mt[3].find_first_of(L'^') != wstring::npos)
        aftermod |= MOD_CONTROL;

      // Alt
      if (mt[3].find_first_of(L'!') != wstring::npos)
        aftermod |= MOD_ALT;

      // Win
      if (mt[3].find_first_of(L'#') != wstring::npos)
        aftermod |= MOD_WIN;
      // -----------------------------------------------



      lpProf->AddRemapKey(code, mod, aftercode, aftermod);

      DebugWriteLine(L"%s(%d) with %d at %s has been remapped to %s(%d) with %d",
        keyname.data(), code, mod,
        lpProf->GetName().data(),
        afterkeyname.data(), aftercode, aftermod);

      continue;
    }


    // ======================
    // �R�}���h���s
    // ======================
    if (matchesw(mt, str2, L"^" + SPACE + KEY + SPACE + L"-->" + SPACE + RUN + ENDOFLINE, true))
    {
      wstring keyname = wstrupper(mt[2]);

      // �Y������L�[��������Ȃ�����
      if (KeyMapList.count(keyname) == 0)
        continue;

      WORD code = CODE_VKCODE(KeyMapList[keyname]);
      BYTE mod = MOD_NONE;
      wstring cmd = mt[3];

      // Shift
      if (mt[1].find_first_of(L'+') != wstring::npos)
        mod |= MOD_SHIFT;

      // Control
      if (mt[1].find_first_of(L'^') != wstring::npos)
        mod |= MOD_CONTROL;

      // Alt
      if (mt[1].find_first_of(L'!') != wstring::npos)
        mod |= MOD_ALT;

      // Win
      if (mt[1].find_first_of(L'#') != wstring::npos)
        mod |= MOD_WIN;


      lpProf->AddRunCmdKey(code, mod, cmd);

      DebugWriteLine(L"%s(%d) with %d at %s will execute \"%s\"", keyname.data(), code, mod, lpProf->GetName().data(), cmd.data());

      continue;
    }
  

    // ==========================================
    // �G���[�s
    // ==========================================
    if (matchw(buf, L"[^ \\t]", true) != L"")
    {
      //StackError(ERROR_ERROR, line, L"�\���������ł��B\n  --> " +buf);
      continue;
    }

  }
}





// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



static UINT ExtractDoubleQuotedString(std::vector<std::wstring>& outarr, wstring& str)
{
  UINT extracted = 0;

  outarr.clear();

  while (1)
  {
    wstring buf = submatchw(str, L"\"([^\"]+?)\"", true);

    if (buf == L"")
      break;

    outarr.emplace_back();
    outarr.back() = buf;

    str = regex_replace(str, std::wregex(wstring(L"\"" + buf + L"\"")), L"");

    extracted++;
  }

  return extracted;
}
