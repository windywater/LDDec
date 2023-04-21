#include "StringHelper.h"
#if !defined(NO_ENCODING_CONVERT)
#include "TextCodecConv.h"
#endif
#include <algorithm>
#include <cctype>

// 编码转换需引入libiconv，将StringHelper移植到其他项目会比较麻烦，对于不需使用编码转换功能的项目，添加这个宏即可
#if !defined(NO_ENCODING_CONVERT)
/*static*/ std::wstring StringHelper::localToWStr(const std::string& str)
{
	return TextCodecConv::bytesToUtf16(str.c_str(), str.size(), "GB18030");
}

/*static*/ std::string StringHelper::wstrToLocal(const std::wstring& wstr)
{
	return TextCodecConv::utf16ToBytes(wstr, "GB18030");
}

/*static*/ std::wstring StringHelper::utf8ToWStr(const std::string& str)
{
	return TextCodecConv::bytesToUtf16(str.c_str(), str.size(), "UTF-8");
}

/*static*/ std::string StringHelper::wstrToUtf8(const std::wstring& wstr)
{
	return TextCodecConv::utf16ToBytes(wstr, "UTF-8");
}

/*static*/ std::string StringHelper::localToUtf8(const std::string& str)
{
	return TextCodecConv::convertBytes(str, "GB18030", "UTF-8");
}

/*static*/ std::string StringHelper::utf8ToLocal(const std::string& str)
{
	return TextCodecConv::convertBytes(str, "UTF-8", "GB18030");
}
#endif

/*static*/ std::string StringHelper::toLower(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });

	return result;
}

/*static*/ std::string StringHelper::toUpper(const std::string& str)
{
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::toupper(c); });

	return result;
}

/*static*/ bool StringHelper::startsWith(const std::string& str, const std::string& sub, bool caseSensitive /*= true*/)
{
	std::string srcStr = str;
	std::string subStr = sub;
	if (!caseSensitive)
	{
		srcStr = StringHelper::toLower(srcStr);
		subStr = StringHelper::toLower(subStr);
	}

	size_t pos = srcStr.find(subStr);
	return (pos == 0);
}

/*static*/ bool StringHelper::endsWith(const std::string& str, const std::string& sub, bool caseSensitive /*= true*/)
{
	std::string srcStr = str;
	std::string subStr = sub;
	if (!caseSensitive)
	{
		srcStr = StringHelper::toLower(srcStr);
		subStr = StringHelper::toLower(subStr);
	}

	size_t pos = srcStr.rfind(subStr);
	if (pos == std::string::npos)
		return false;

	return (pos == srcStr.size() - subStr.size());
}

/*static*/ bool StringHelper::contains(const std::string& str, const std::string& sub, bool caseSensitive /*= true*/)
{
	std::string srcStr = str;
	std::string subStr = sub;
	if (!caseSensitive)
	{
		srcStr = StringHelper::toLower(srcStr);
		subStr = StringHelper::toLower(subStr);
	}

	size_t pos = srcStr.rfind(subStr);
	if (pos == std::string::npos)
		return false;

	return true;
}

static bool replaceOne(std::string& str, const std::string& before, const std::string& after)
{
	size_t startPos = str.find(before);
	if (startPos == std::string::npos)
		return false;

	str.replace(startPos, before.size(), after);
	return true;
}

/*static*/ std::string StringHelper::replace(const std::string& str, const std::string& before, const std::string& after)
{
	if (before.empty())
		return str;

	std::string newStr = str;

	size_t startPos = 0;
	while ((startPos = newStr.find(before, startPos)) != std::string::npos)
	{
		newStr.replace(startPos, before.size(), after);
		startPos += after.length();
	}

	return newStr;
}

/*static*/ std::string StringHelper::join(const std::vector<std::string>& strs, const std::string& separator)
{
	std::string result;
	size_t size = strs.size();
	for (size_t i = 0; i < size; i++)
	{
		result += strs[i];

		if (i != size - 1)
			result += separator;
	}

	return result;
}

/*static*/ std::vector<std::string> StringHelper::split(const std::string& str, const std::string& delimiter, bool skipEmpty /*= false*/)
{
	std::vector<std::string> results;

	if (delimiter.empty())
		return std::vector<std::string>{str};

	std::string::size_type pos1 = 0;
	std::string::size_type pos2 = str.find(delimiter);

	while (pos2 != std::string::npos)
	{
		if (!skipEmpty || (pos1 != pos2))
			results.emplace_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + delimiter.size();
		pos2 = str.find(delimiter, pos1);
	}

	if (pos1 != str.length())
		results.emplace_back(str.substr(pos1));

	return results;
}


