#pragma once
#include <string>
#include <vector>

std::string match(const std::string, const std::string, bool);
std::wstring matchw(const std::wstring, const std::wstring, bool);
std::wstring submatchw(const std::wstring, const std::wstring, bool);
bool matchesw(std::vector<std::wstring>&, const std::wstring, const std::wstring, bool);
std::wstring replacew(const std::wstring, const std::wstring, const std::wstring, bool);
