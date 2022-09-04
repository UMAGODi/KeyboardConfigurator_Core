#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <mutex>
#include <filesystem>
#include <unordered_map>
#include "KeyCode.h"

//using std::wstring;
//using std::filesystem::path;
//using std::vector;

using namespace std;
using std::filesystem::path;



#define JSON_GENERAL "General"
#define JSON_PROFNAME "ProfName"
#define JSON_PROFCOLOR "ProfColor"
#define JSON_ASSIGN "Assign"
#define JSON_APPEXE "Exe"
#define JSON_APPTITLE "Title"
#define JSON_FUNCTIONS "Functions"

enum
{
  TYPE_DISABLE,
  TYPE_THROUGH,
  TYPE_REMAP,
  TYPE_RUN
};

#define MOD_NONE    0
#define MOD_SHIFT	  1
#define MOD_CONTROL 2
#define MOD_ALT		  4
#define MOD_WIN		  8
#define MOD_SOLO    64

#define ISPRESSED_SHIFT(bit)   (BOOL(bit & MOD_SHIFT))
#define ISPRESSED_CONTROL(bit) (BOOL(bit & MOD_CONTROL))
#define ISPRESSED_ALT(bit)     (BOOL(bit & MOD_ALT))
#define ISPRESSED_WIN(bit)     (BOOL(bit & MOD_WIN))


// ------------------------------------------------
// KeyInfo
// 各キーに割り当てられた機能を管理
typedef struct
{
  UINT Type;

  wstring execode;
  DWORD RemappedKey;
  BYTE  RemappedMod;

}KEYINFO, *LPKEYINFO;


// Key[code][mod]
typedef unordered_map<WORD, unordered_map<BYTE, LPKEYINFO>> KEYLISTMAP;


// ------------------------------------------------
// KbcProfile
// プロファイル

class KbcProfile
{
private:
  wstring Name;
  unordered_map<wstring, bool> AppExe;
  unordered_map<wstring, bool> AppTitle;


  
  KEYLISTMAP mapKeyList;

  // Func
  void DeleteAllKeyInfo(void);


public:
  KbcProfile();
  KbcProfile(wstring);
  ~KbcProfile();


  // Assignment
  void AddExeAssign(wstring);
  void AddTitleAssign(const wstring);
  BOOL CheckExistAssign(wstring, const wstring);
  BOOL CheckExistAssignExe(wstring);
  BOOL CheckExistAssignTitle(const wstring);

  // Key Func
  void AddDisableKey(const WORD, BYTE);
  void AddThroughKey(const WORD, BYTE);
  void AddRemapKey(const WORD, BYTE, const DWORD, const BYTE);
  void AddRunCmdKey(const WORD, BYTE, const wstring);
  
  // Misc
  wstring GetName(void);
  bool Using(void);
  KEYLISTMAP& GetlpKeyList(void);
  BOOL ProcessKey(const WORD, BYTE, const BOOL);
  void ReleaseAllKeys(void);

};


class ProfileMaster
{
private:
  unordered_map<wstring, KbcProfile*> lpProfiles;
  KbcProfile* CurrentProf;

public:
  ProfileMaster(void);
  ~ProfileMaster(void);

  KbcProfile* AddProfile(const wstring);
  bool RemoveProfile(const wstring);
  void RemoveAllProfiles(void);

  KbcProfile* Default(void);
  KbcProfile* Global(void);
  KbcProfile* Current(void);
  KbcProfile* Profile(const wstring);
  KbcProfile* FindByExe(const wstring);
  KbcProfile* FindByTitle(const wstring);
  KbcProfile* FindByExeOrTitle(const wstring, const wstring);

  void SwitchProfile(const wstring, const wstring);
  bool CopyProfile(const wstring, const wstring);
};

