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
// ���蓖�Ēǉ�
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
// ���蓖�Ă���v���邩�ǂ������`�F�b�N
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
// �L�[�ǉ�
// ==============================
void KbcProfile::AddDisableKey(const WORD code, BYTE mod)
{
  // �V�K
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_DISABLE;

}


void KbcProfile::AddThroughKey(const WORD code, BYTE mod)
{
  // �V�K
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_THROUGH;

}

void KbcProfile::AddRemapKey(
  const WORD code, BYTE mod,
  const DWORD newkey, const BYTE newmod)
{
  //�V�K
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_REMAP;
  mapKeyList[code][mod]->RemappedKey = newkey;
  mapKeyList[code][mod]->RemappedMod = newmod;
}



void KbcProfile::AddRunCmdKey(const WORD code, BYTE mod, const wstring cmd)
{
  //�V�K
  if (mapKeyList.count(code) == 0 || mapKeyList[code].count(mod) == 0)
    mapKeyList[code][mod] = (LPKEYINFO) new KEYINFO;

  mapKeyList[code][mod]->Type = TYPE_RUN;
  mapKeyList[code][mod]->execode = cmd;
}




// ==============================
// �����o�ϐ�
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
// �L�[������
// ==============================

BOOL KbcProfile::ProcessKey(const WORD wVk, BYTE mod, const BOOL isUp)
{

  // mod ��KBC�ȊO�ŉ�����Ă�C���L�[�i�����I�ɉ�����Ă�L�[�j�r�b�g���
  // ������ mod �ɂ� MOD_SOLO ���͊܂܂�Ȃ�


  // '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
  // �P�ƃL�[ (MOD_NONE) �͑��̏C���L�[�̉����Ɋւ��Ȃ����A
  // �\���L�[ (MOD_SOLO) �͂��ׂĂ̏C���L�[�͉�������Ă��Ă͂����Ȃ�
  // 
  // ���Ƃ��ΒP�ƃL�[�� A �𖳌������Ă����Ƃ��A
  // Ctrl+A �� Ctrl+Shift+A �Ȃǂ�����������邪�A�\���L�[�Œ�`���Ă����Ƃ���
  // A �P�̂̓��͖͂���������邪�ACtr+A �� Ctrl+Shift+A �Ȃǂ̓X���[�����
  // '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

  /*
  | ====================================================================================================================
  | �C���L�[�� MOD_NONE�@       |                 �͂�                   |                 �@   �������@                      |
  | ������Ă�C���L�[�Ɉ�v�����` |                 ����                   |          ����          |          �@�Ȃ�           |
  | �P�ƃL�[�̒�`�@             |     �����@   |     ����@   |     �Ȃ�    |          ����          |   �@����     |     �Ȃ�    |
  | �\���L�[�̒�`�@             |     ����   �@|            �Ȃ��@         |                 �@    �����@                     �@| 
  | --------------------------------------------------------------------------------------------------------------------
  |                          | �\���L�[���g�p | �P�ƃL�[���g�p |   return   | �C���L�[�Ɉ�v�����`���g�p | �P�ƃL�[���g�p |   return   |
  | ====================================================================================================================


  �yreturn�z------------------------------
  �E���������L�[�̒�`���Ȃ��Ƃ�
  �Emod == MOD_NONE�ŁAmapKeyList[wVk].count(MOD_SOLO) == 0�ŁAmapKeyList[wVk].count(MOD_NONE) == 0 �̂Ƃ�
  �i�C���L�[�����������Ă��Ȃ����A�\���L�[���P�ƃL�[����`����Ă��Ȃ��Ƃ��j
  
  �Emod != MOD_NONE�ŁAmapKeyList[wVk].count(mod) == 0�ŁAmapKeyList[wVk].count(MOD_NONE) == 0 �̂Ƃ�
  �i�C���L�[��������Ă��邪�A�Y�������`���Ȃ��A�܂��P�ƃL�[����`����Ă��Ȃ��Ƃ��j


  �y�C���L�[�Ɉ�v�����`���g�p�z---------------
  �E��L�ȊO��
  �Emod != MOD_NONE�ŁAmapKeyList[wVk].count(mod) != 0 �̂Ƃ��i�C���L�[��������Ă��āA���̒�`������Ƃ��j

  �y�\���L�[���g�p�z-------------------------
  �E��L�ȊO��
  �Emod == MOD_NONE�ŁAmapKeyList[wVk].count(MOD_SOLO) != 0 �̂Ƃ��i�C���L�[�����������Ă��Ȃ��āA�\���L�[�̒�`������Ƃ��j

  �y�P�ƃL�[���g�p�z-------------------------
  �E��L�ȊO��
  �EmapKeyList[wVk].count(MOD_NONE) != 0 �̂Ƃ��i�P�ƃL�[����`����Ă���Ƃ��j
  */


  // �yreturn�z
  if (mapKeyList.count(wVk) == 0)
    return FALSE;


  
  if (isUp == FALSE)
  {

    // �yreturn�z
    if ((mod == MOD_NONE && mapKeyList[wVk].count(MOD_SOLO) == 0 && mapKeyList[wVk].count(MOD_NONE) == 0)
      || (mod != MOD_NONE && mapKeyList[wVk].count(mod) == 0 && mapKeyList[wVk].count(MOD_NONE) == 0))
      return FALSE;


    // �y�C���L�[�Ɉ�v�����`���g�p�z
    if (mod != MOD_NONE && mapKeyList[wVk].count(mod != 0))
    {

    }

    // �y�\���L�[���g�p�z
    else if (mod == MOD_NONE && mapKeyList[wVk].count(MOD_SOLO) != 0)
    {
      mod = MOD_SOLO;
    }

    // �y�P�ƃL�[���g�p�z
    else if (mapKeyList[wVk].count(MOD_NONE) != 0)
    {
      mod = MOD_NONE;
    }

    // ���邩�킩��Ȃ����Ǘ�O
    else
    {
      return FALSE;
    }

    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // �����Ȃǂ� mod �g���̂ŏ�ł����ƓK�؂� mod ��
    // ���Ă����Ȃ��Ɨ�O�ł邩�璍��
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    KEYINFO& key = *mapKeyList[wVk][mod];
    UINT& type = key.Type;

    // ------------------------
    // ������
    // ------------------------
    if (type == TYPE_DISABLE)
      return TRUE;

    // ------------------------
    // ���}�b�v
    // ------------------------
    else if (type == TYPE_REMAP)
    {

      DWORD& afterkey = key.RemappedKey;
      BYTE& aftermod = key.RemappedMod;

      ACTIONLIST& refact = CreateActionList();



      // �C���L�[

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

      // ��
      // ���M��A�v���P�[�V�����ɂ���Ă�2�ȏ�̃L�[�����S�ɓ����ɑ��M����ƁA�O�̃L�[�̏����̎��s�r���Ȃǂ�
      // �ォ�瑗�����L�[����������Ă��܂����Ƃ����� (���炭�J�������A�l�Ԃɂ͕s�\�Ȃ��܂�ɂ��������铯��������z�肵�Ă��Ȃ�����)
      // �܂��̓`���^�����O�L�����Z���ȂǂŒe����邽��
      // 
      // ���̂��߂�����x�̒x����^���邱�Ƃł��̖�����������


      // ###################
      // �{��
      // ###################
      AddKeySendAction(refact, afterkey, isUp);


      // �����グ�����݂��Ȃ��L�[�͓����ɉ����グ�����M����
      if (wVk == VK_WHEELUP || wVk == VK_WHEELDOWN
        || wVk == VK_HWHEELLEFT || wVk == VK_HWHEELRIGHT
        || wVk == VK_OEM_AUTO || wVk == VK_OEM_ENLW || wVk == VK_OEM_ATTN)
      {
        // ��������Ə�������Ȃ����Ƃ�����̂�
        AddDelayAction(refact, MINIMAL_DELAY);
        AddKeySendAction(refact, afterkey, TRUE);
  
        // �C���L�[
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

      // delete ���邽�� TRUE
      ExecuteAction(refact, TRUE);

      return TRUE;
    }

    // ------------------------
    // �R�}���h���s
    // ------------------------
    else if (type == TYPE_RUN && isUp == FALSE)
    {
      Execute(key.execode);

      return TRUE;
    }
  }


  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // ����͏����������Ⴄ
  // �Ⴆ�� Ctr+A �� Shift+Tab ���o�͂����`���������ꍇ�ACtrl+A ������������ Ctrl �𗣂������� A �𗣂����ꍇ�A
  // A �𗣂������_�� Ctrl �͉�����ĂȂ�����A�������Ɠ��l�ɏ������悤�Ƃ���� A �P�̂̏������Ă΂�Ă��܂�
  // 
  // ���̂��߉��㎞�̏����͏C���L�[�ɂ�炸�A�S�Ă̏C���L�[�̃p�^�[���ŉ��㏈�����s��
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  else
  {
    BYTE bit = MOD_NONE;
    ACTIONLIST& refact = CreateActionList();

    

    // --------------------------
    // ���オ�K�v�ȏC���L�[�𐮗�����
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


    // ����ɕK�v�ȃL�[�����ۂɉ�����Ă��Ȃ���Ή��グ��
    // (�����ŉ�����Ă�̂ɉ����񑗂�Ƃ��������Ȃ�)
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


    // delete ����̂� TRUE
    ExecuteAction(refact, TRUE);
  }
  

  return FALSE;
}


// ==============================
// �S�L�[����
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

        // �{��
        if (GETKEYSTATE(CODE_VKCODE(key)))
        {

          AddKeySendAction(refact, key, TRUE);

          // �C���L�[
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
// �v���t�@�C���ǉ�
// ====================================
KbcProfile* ProfileMaster::AddProfile(const wstring name)
{
  if (lpProfiles.count(name) == 0)
    lpProfiles[name] = new KbcProfile(name);

  return lpProfiles[name];
}

// ====================================
// �v���t�@�C���폜
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
// �v���t�@�C���擾
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
    || lpProfiles[dest]->Using() ) //�L�[�o�^���import�ł��Ȃ�
    return false;

  KEYLISTMAP& fromKeyList = lpProfiles[from]->GetlpKeyList();
  KEYLISTMAP& destKeyList = lpProfiles[dest]->GetlpKeyList();

  for (auto m : fromKeyList)
  {
    for (auto n : m.second)
    {
      // new ����̂� destKeyList[m.first][n.first] ����`�ς݂ł����Ă͂Ȃ�Ȃ�
      destKeyList[m.first][n.first] = new KEYINFO;
      *(destKeyList[m.first][n.first]) = *(fromKeyList[m.first][n.first]);
    }
  }

}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

