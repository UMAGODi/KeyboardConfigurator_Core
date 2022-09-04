#pragma comment(lib, "shlwapi")

#include <utility>
#include <ShlObj.h>
#include <climits>
#include <shlwapi.h>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "KbcProfile.h"
#include "SendKey.h"
#include "Common.h"
#include "Debug.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

#define DEFAULT_PROF L"Default"
#define GLOBAL_PROF  L"Global"
#define MINIMAL_DELAY 10 // in ms

#define GETKEYSTATE(code) bool(GetKeyState(int(code)) < 0)


// -----------------------------------------------------------------------------------------------------------------------------------

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// KbcProfile
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

KbcProfile::KbcProfile(void)
{
  Name = L"";
}

KbcProfile::KbcProfile(wstring name)
{
  Name = name;
}

KbcProfile::~KbcProfile(void)
{
  this->DeleteAllKeyInfo();
}


void KbcProfile::DeleteAllKeyInfo(void)
{
  for (auto& key : mapKeyList)
  {
    for (auto key2 : key.second)
    {
      delete key2.second;
    }
  }
}

// ==============================
// 割り当て追加
// ==============================
void KbcProfile::AddExeAssign(wstring exe)
{
  std::transform(exe.cbegin(), exe.cend(), exe.begin(), tolower_);

  AppExe[exe] = true;
}

void KbcProfile::AddTitleAssign(const wstring title)
{
  AppTitle[title] = true;
}


// ==============================
// 割り当てが一致するかどうかをチェック
// ==============================
BOOL KbcProfile::CheckExistAssign(wstring exe, const wstring title)
{
  if (this->CheckExistAssignExe(exe))
    return TRUE;

  if (this->CheckExistAssignTitle(title))
    return TRUE;

  return FALSE;
}


BOOL KbcProfile::CheckExistAssignExe(wstring exe)
{
  if (AppExe.size() == 0)
    return FALSE;

  std::transform(exe.cbegin(), exe.cend(), exe.begin(), tolower_);

  if (AppExe.count(exe))
    return TRUE;

  return FALSE;
}

BOOL KbcProfile::CheckExistAssignTitle(const wstring title)
{
  if (AppTitle.size() == 0)
    return FALSE;

  if (AppTitle.find(title) != AppTitle.end())
    return TRUE;

  return FALSE;
}


// ==============================
// キー追加
// ==============================
void KbcProfile::AddDisableKey(const WORD code, BYTE mod)
{
  // 新規
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_DISABLE;

}


void KbcProfile::AddThroughKey(const WORD code, BYTE mod)
{
  // 新規
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_THROUGH;

}

void KbcProfile::AddRemapKey(
  const WORD code, BYTE mod,
  const DWORD newkey, const BYTE newmod)
{
  //新規
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_REMAP;
  mapKeyList[code][mod]->RemappedKey = newkey;
  mapKeyList[code][mod]->RemappedMod = newmod;
}



void KbcProfile::AddRunCmdKey(const WORD code, BYTE mod, const wstring cmd)
{
  //新規
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_RUN;
  mapKeyList[code][mod]->execode = cmd;
}




// ==============================
// メンバ変数
// ==============================
wstring KbcProfile::GetName(void)
{
  return Name;
}

bool KbcProfile::Using(void)
{
  return mapKeyList.size();
}


KEYLISTMAP& KbcProfile::GetlpKeyList(void)
{
  return mapKeyList;
}

// ==============================
// キーを処理
// ==============================

BOOL KbcProfile::ProcessKey(const WORD wVk, BYTE mod, const BOOL isUp)
{

  // mod はKBC以外で押されてる修飾キー（物理的に押されてるキー）ビット情報
  // 引数の mod には MOD_SOLO 情報は含まれない


  // '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  // 単独キー (MOD_NONE) は他の修飾キーの押下に関わらないが、
  // ソロキー (MOD_SOLO) はすべての修飾キーは押下されていてはいけない
  // 
  // たとえば単独キーで A を無効化していたとき、
  // Ctrl+A や Ctrl+Shift+A なども無効化されるが、ソロキーで定義していたときは
  // A 単体の入力は無効化されるが、Ctr+A や Ctrl+Shift+A などはスルーされる
  // '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

  /*
  | ====================================================================================================================
  | 修飾キーは MOD_NONE　       |                 はい                   |                 　   いいえ　                      |
  | 押されてる修飾キーに一致する定義 |                 無視                   |          ある          |          　ない           |
  | 単独キーの定義　             |     無視　   |     ある　   |     ない    |          無視          |   　ある     |     ない    |
  | ソロキーの定義　             |     ある   　|            ない　         |                 　    無視　                     　| 
  | --------------------------------------------------------------------------------------------------------------------
  |                          | ソロキーを使用 | 単独キーを使用 |   return   | 修飾キーに一致する定義を使用 | 単独キーを使用 |   return   |
  | ====================================================================================================================


  【return】------------------------------
  ・そもそもキーの定義がないとき
  ・mod == MOD_NONEで、mapKeyList[wVk].count(MOD_SOLO) == 0で、mapKeyList[wVk].count(MOD_NONE) == 0 のとき
  （修飾キーが一つも押されていないが、ソロキーも単独キーも定義されていないとき）
  
  ・mod != MOD_NONEで、mapKeyList[wVk].count(mod) == 0で、mapKeyList[wVk].count(MOD_NONE) == 0 のとき
  （修飾キーが押されているが、該当する定義がなく、また単独キーも定義されていないとき）


  【修飾キーに一致する定義を使用】---------------
  ・上記以外で
  ・mod != MOD_NONEで、mapKeyList[wVk].count(mod) != 0 のとき（修飾キーが押されていて、その定義があるとき）

  【ソロキーを使用】-------------------------
  ・上記以外で
  ・mod == MOD_NONEで、mapKeyList[wVk].count(MOD_SOLO) != 0 のとき（修飾キーが一つも押されていなくて、ソロキーの定義があるとき）

  【単独キーを使用】-------------------------
  ・上記以外で
  ・mapKeyList[wVk].count(MOD_NONE) != 0 のとき（単独キーが定義されているとき）
  */


  // 【return】
  if (mapKeyList.count(wVk) == 0)
    return FALSE;


  
  if (isUp == FALSE)
  {

    // 【return】
    if ((mod == MOD_NONE && mapKeyList[wVk].count(MOD_SOLO) == 0 && mapKeyList[wVk].count(MOD_NONE) == 0)
      || (mod != MOD_NONE && mapKeyList[wVk].count(mod) == 0 && mapKeyList[wVk].count(MOD_NONE) == 0))
      return FALSE;


    // 【修飾キーに一致する定義を使用】
    if (mod != MOD_NONE && mapKeyList[wVk].count(mod != 0))
    {

    }

    // 【ソロキーを使用】
    else if (mod == MOD_NONE && mapKeyList[wVk].count(MOD_SOLO) != 0)
    {
      mod = MOD_SOLO;
    }

    // 【単独キーを使用】
    else if (mapKeyList[wVk].count(MOD_NONE) != 0)
    {
      mod = MOD_NONE;
    }

    // あるかわからないけど例外
    else
    {
      return FALSE;
    }

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // ここなどで mod 使うので上でちゃんと適切な mod に
    // しておかないと例外でるから注意
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    KEYINFO& key = *mapKeyList[wVk][mod];
    UINT& type = key.Type;

    // ------------------------
    // 無効化
    // ------------------------
    if (type == TYPE_DISABLE)
      return TRUE;

    // ------------------------
    // リマップ
    // ------------------------
    else if (type == TYPE_REMAP)
    {

      DWORD& afterkey = key.RemappedKey;
      BYTE& aftermod = key.RemappedMod;

      ACTIONLIST& refact = CreateActionList();



      // 修飾キー

      if ((aftermod & MOD_SHIFT) && !(GETKEYSTATE(VK_LSHIFT) || GETKEYSTATE(VK_RSHIFT)))
      {
        AddKeySendAction(refact, KEY_LSHIFT, FALSE);
        AddDelayAction(refact, MINIMAL_DELAY);
      }

      if ((aftermod & MOD_CONTROL) && !(GETKEYSTATE(VK_LCONTROL) || GETKEYSTATE(VK_RCONTROL)))
      {
        AddKeySendAction(refact, KEY_LCONTROL, FALSE);
        AddDelayAction(refact, MINIMAL_DELAY);
      }

      if ((aftermod & MOD_ALT) && !(GETKEYSTATE(VK_LMENU) || GETKEYSTATE(VK_RMENU)))
      {
        AddKeySendAction(refact, KEY_LMENU, FALSE);
        AddDelayAction(refact, MINIMAL_DELAY);
      }

      if ((aftermod & MOD_WIN) && !(GETKEYSTATE(VK_LWIN) || GETKEYSTATE(VK_RWIN)))
      {
        AddKeySendAction(refact, KEY_LWIN, FALSE);
        AddDelayAction(refact, MINIMAL_DELAY);
      }

      // ↑
      // 送信先アプリケーションによっては2つ以上のキーを完全に同時に送信すると、前のキーの処理の実行途中などで
      // 後から送ったキーが無視されてしまうことがある (恐らく開発側が、人間には不可能なあまりにも早すぎる同時押しを想定していないため)
      // またはチャタリングキャンセラなどで弾かれるため
      // 
      // そのためある程度の遅延を与えることでこの問題を解決する


      // ###################
      // 本命
      // ###################
      AddKeySendAction(refact, afterkey, isUp);


      // 押し上げが存在しないキーは同時に押し上げも送信する
      if (wVk == VK_WHEELUP || wVk == VK_WHEELDOWN
        || wVk == VK_HWHEELLEFT || wVk == VK_HWHEELRIGHT
        || wVk == VK_OEM_AUTO || wVk == VK_OEM_ENLW || wVk == VK_OEM_ATTN)
      {
        // 早すぎると処理されないことがあるので
        AddDelayAction(refact, MINIMAL_DELAY);
        AddKeySendAction(refact, afterkey, TRUE);
  
        // 修飾キー
        if ((aftermod & MOD_SHIFT) && !ISPRESSED_SHIFT(mod))
        {
          AddDelayAction(refact, MINIMAL_DELAY);
          AddKeySendAction(refact, KEY_LSHIFT, TRUE);
        }

        if ((aftermod & MOD_CONTROL) && !ISPRESSED_CONTROL(mod))
        {
          AddDelayAction(refact, MINIMAL_DELAY);
          AddKeySendAction(refact, KEY_LCONTROL, TRUE);
        }

        if ((aftermod & MOD_ALT) && !ISPRESSED_ALT(mod))
        {
          AddDelayAction(refact, MINIMAL_DELAY);
          AddKeySendAction(refact, KEY_LMENU, TRUE);
        }

        if ((aftermod & MOD_WIN) && !ISPRESSED_WIN(mod))
        {
          AddDelayAction(refact, MINIMAL_DELAY);
          AddKeySendAction(refact, KEY_LWIN, TRUE);
        }
      }

      // delete するため TRUE
      ExecuteAction(refact, TRUE);

      return TRUE;
    }

    // ------------------------
    // コマンド実行
    // ------------------------
    else if (type == TYPE_RUN && isUp == FALSE)
    {
      Execute(key.execode);

      return TRUE;
    }
  }


  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // 押上は少し処理が違う
  // 例えば Ctr+A で Shift+Tab を出力する定義があった場合、Ctrl+A を押し続けて Ctrl を離したあと A を離した場合、
  // A を離した時点で Ctrl は押されてないから、押下時と同様に処理しようとすると A 単体の処理が呼ばれてしまう
  // 
  // そのため押上時の処理は修飾キーによらず、全ての修飾キーのパターンで押上処理を行う
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    BYTE bit = MOD_NONE;
    ACTIONLIST& refact = CreateActionList();

    

    // --------------------------
    // 押上が必要な修飾キーを整理する
    // --------------------------
    for (auto i : mapKeyList[wVk])
    {
      if (i.second->Type != TYPE_REMAP)
        continue;

      AddKeySendAction(refact, i.second->RemappedKey, TRUE);

      if (i.second->RemappedMod & MOD_SHIFT)
        bit |= MOD_SHIFT;

      if (i.second->RemappedMod & MOD_CONTROL)
        bit |= MOD_CONTROL;
    
      if (i.second->RemappedMod & MOD_ALT)
        bit |= MOD_ALT;
    
      if (i.second->RemappedMod & MOD_WIN)
        bit |= MOD_WIN;
    }


    // 押上に必要なキーが実際に押されていなければ押上げる
    // (物理で押されてるのに押上情報送るとおかしくなる)
    if ((bit & MOD_SHIFT) && !ISPRESSED_SHIFT(mod))
    {
      AddDelayAction(refact, MINIMAL_DELAY);
      AddKeySendAction(refact, KEY_LSHIFT, TRUE);
    }

    if ((bit & MOD_CONTROL) && !ISPRESSED_CONTROL(mod))
    {
      AddDelayAction(refact, MINIMAL_DELAY);
      AddKeySendAction(refact, KEY_LCONTROL, TRUE);
    }

    if ((bit & MOD_ALT) && !ISPRESSED_ALT(mod))
    {
      AddDelayAction(refact, MINIMAL_DELAY);
      AddKeySendAction(refact, KEY_LMENU, TRUE);
    }

    if ((bit & MOD_WIN) && !ISPRESSED_WIN(mod))
    {
      AddDelayAction(refact, MINIMAL_DELAY);
      AddKeySendAction(refact, KEY_LWIN, TRUE);
    }


    // delete するので TRUE
    ExecuteAction(refact, TRUE);
  }
  

  return FALSE;
}


// ==============================
// 全キー押上
// ==============================
void KbcProfile::ReleaseAllKeys(void)
{
  DebugWriteLine(L"Releasing");

  ACTIONLIST& refact = CreateActionList();

  for (auto& key : mapKeyList)
  {
    for (auto key2 : key.second)
    {
      if (key2.second->Type == TYPE_REMAP)
      {
        DWORD key = key2.second->RemappedKey;
        BYTE  mod = key2.second->RemappedMod;

        // 本体
        if (GETKEYSTATE(CODE_VKCODE(key)))
        {

          AddKeySendAction(refact, key, TRUE);

          // 修飾キー
          if ((mod & MOD_SHIFT) && (GETKEYSTATE(VK_LSHIFT)))
            AddKeySendAction(refact, KEY_LSHIFT, TRUE);

          if ((mod & MOD_CONTROL) && (GETKEYSTATE(VK_LCONTROL)))
            AddKeySendAction(refact, KEY_LCONTROL, TRUE);

          if ((mod & MOD_ALT) && (GETKEYSTATE(VK_LMENU)))
            AddKeySendAction(refact, KEY_LMENU, TRUE);

          if ((mod & MOD_WIN) && (GETKEYSTATE(VK_LWIN)))
            AddKeySendAction(refact, KEY_LWIN, TRUE);

        }

      }
    }
  }

  ExecuteAction(refact, TRUE);
}




// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%






ProfileMaster::ProfileMaster(void)
{
  this->AddProfile(GLOBAL_PROF);
  this->AddProfile(DEFAULT_PROF);

  CurrentProf = this->Default();
}

ProfileMaster::~ProfileMaster(void)
{
  this->RemoveAllProfiles();
}

// ====================================
// プロファイル追加
// ====================================
KbcProfile* ProfileMaster::AddProfile(const wstring name)
{
  if (lpProfiles.count(name) == 0)
    lpProfiles[name] = new KbcProfile(name);

  return lpProfiles[name];
}

// ====================================
// プロファイル削除
// ====================================
bool ProfileMaster::RemoveProfile(const wstring name)
{
  if (lpProfiles.count(name) == 0)
    return false;

  delete lpProfiles[name];
  lpProfiles.erase(name);

  return true;
}


void ProfileMaster::RemoveAllProfiles(void)
{
  for (auto i : lpProfiles)
  {
    delete i.second;
  }

  lpProfiles.clear();
}


// ====================================
// プロファイル取得
// ====================================
KbcProfile* ProfileMaster::Default(void)
{
  return this->Profile(DEFAULT_PROF);
}

KbcProfile* ProfileMaster::Global(void)
{
  return this->Profile(GLOBAL_PROF);
}

KbcProfile* ProfileMaster::Current(void)
{
  return CurrentProf;
}

KbcProfile* ProfileMaster::Profile(wstring name)
{
  if (lpProfiles.count(name))
    return lpProfiles[name];

  return nullptr;
}

KbcProfile* ProfileMaster::FindByExe(wstring exe)
{
  for (auto i : lpProfiles)
  {
    if (i.second->CheckExistAssignExe(exe))
      return i.second;
  }

  return nullptr;
}

KbcProfile* ProfileMaster::FindByTitle(wstring title)
{
  for (auto i : lpProfiles)
  {
    if (i.second->CheckExistAssignTitle(title))
      return i.second;
  }

  return nullptr;
}
KbcProfile* ProfileMaster::FindByExeOrTitle(wstring exe, wstring title)
{
  for (auto i : lpProfiles)
  {
    if (i.second->CheckExistAssign(exe, title))
      return i.second;
  }

  return nullptr;
}


void ProfileMaster::SwitchProfile(wstring exe, wstring title)
{
  static mutex mtx;
  mtx.lock();

  KbcProfile* old = CurrentProf;
  CurrentProf = this->Default();

  for (auto i : lpProfiles)
  {
    if (i.second->CheckExistAssign(exe, title))
    {
      CurrentProf = i.second;
      break;
    }
  }


  old->ReleaseAllKeys();
  DebugWriteLine(L"Profile has been switched to <%s>", CurrentProf->GetName().data());

  mtx.unlock();
}


bool ProfileMaster::CopyProfile(const wstring from, const wstring dest)
{
  if (lpProfiles.count(from) == 0 || lpProfiles.count(dest) == 0
    || lpProfiles[dest]->Using() ) //キー登録後はimportできない
    return false;

  KEYLISTMAP& fromKeyList = lpProfiles[from]->GetlpKeyList();
  KEYLISTMAP& destKeyList = lpProfiles[dest]->GetlpKeyList();

  for (auto m : fromKeyList)
  {
    for (auto n : m.second)
    {
      // new するので destKeyList[m.first][n.first] が定義済みであってはならない
      destKeyList[m.first][n.first] = new KEYINFO;
      *(destKeyList[m.first][n.first]) = *(fromKeyList[m.first][n.first]);
    }
  }

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

