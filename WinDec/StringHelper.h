#pragma once
#include <string>
#include <vector>

class StringHelper
{
public:
#if !defined(NO_ENCODING_CONVERT)
	static std::wstring localToWStr(const std::string& str);
	static std::string wstrToLocal(const std::wstring& wstr);

	static std::wstring utf8ToWStr(const std::string& str);
	static std::string wstrToUtf8(const std::wstring& wstr);

	static std::string localToUtf8(const std::string& str);
	static std::string utf8ToLocal(const std::string& str);
#endif

	static std::string toLower(const std::string& str);
	static std::string toUpper(const std::string& str);

	static bool startsWith(const std::string& str, const std::string& sub, bool caseSensitive = true);
	static bool endsWith(const std::string& str, const std::string& sub, bool caseSensitive = true);
	static bool contains(const std::string& str, const std::string& sub, bool caseSensitive = true);

	static std::string replace(const std::string& str, const std::string& before, const std::string& after);
	static std::string join(const std::vector<std::string>& strs, const std::string& separator);
	static std::vector<std::string> split(const std::string& str, const std::string& delimiter, bool skipEmpty = false);

};
