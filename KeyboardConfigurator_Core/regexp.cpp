#include <string>
#include <regex>
#include "Debug.h"
#include "regexp.h"


std::string match(const std::string tgt, const std::string pattern, bool ignore)
{
  std::smatch match;


  if (std::regex_search(
    tgt, match, 
    std::regex(pattern, std::regex_constants::syntax_option_type(std::regex_constants::icase * ignore))))
  {
    return match.str();
  }

  return "";
}


std::wstring matchw(const std::wstring tgt, const std::wstring pattern, bool ignore)
{
  std::wsmatch wmatch;


  if (std::regex_search(
    tgt, wmatch, 
    std::wregex(pattern, std::regex_constants::syntax_option_type(std::regex_constants::icase * ignore))))
  {
    return wmatch.str();
  }

  return L"";
}



std::wstring submatchw(const std::wstring tgt, const std::wstring pattern, bool ignore)
{
  std::wsmatch wmatch;


  if (std::regex_search(
    tgt, wmatch, 
    std::wregex(pattern, std::regex_constants::syntax_option_type(std::regex_constants::icase * ignore))))
  {

    if (wmatch.size() > 1)
      return wmatch[1].str();
  }

  return L"";
}




bool matchesw(std::vector<std::wstring> &res, const std::wstring tgt, const std::wstring pattern, bool ignore)
{
  std::wsmatch wmatch;


  if (std::regex_search(
    tgt, wmatch, 
    std::wregex(pattern, std::regex_constants::syntax_option_type(std::regex_constants::icase * ignore))))
  {

    res.clear();
    res.reserve(wmatch.size());

    for (auto m : wmatch)
    {
      res.emplace_back();
      res.back() = m.str();
    }
    return true;
  }

  return false;
}





std::wstring replacew(const std::wstring tgt, const std::wstring pattern, const std::wstring newstr, bool ignore)
{
  return std::regex_replace(tgt, std::wregex(pattern), newstr);
}


