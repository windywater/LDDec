#include "FileHelper.h"
#include "StringHelper.h"
#include <fstream>
#include <stack>
#include <io.h>
#include <direct.h>
#include <cstdio>
#include <Windows.h>
#include <shlobj.h>

/*static*/ std::string FileHelper::readFromFile(const std::string& fileName, unsigned int size /*= -1*/)
{
	std::fstream fs;
	std::string content;

	fs.open(fileName.c_str(), std::ios_base::in | std::ifstream::binary);
	fs.seekg(0, std::fstream::end);
	__int64 length = (int)fs.tellg();
	if (length < 0)
		return content;

	fs.seekg(0, std::fstream::beg);
	if (size == -1)
		size = (unsigned int)length;

	char* buf = new char[size];
	fs.read(buf, size);
	content.assign(buf, size);
	delete[] buf;

	fs.close();

	return content;
}

/*static*/ void FileHelper::writeToFile(const std::string& fileName, const std::string& content)
{
	std::fstream fs;
	fs.open(fileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ifstream::binary);
	fs.write(content.c_str(), content.size());
	fs.close();
}

/*static*/ std::string FileHelper::normalizePath(const std::string& file)
{
	return StringHelper::replace(file, "/", "\\");
}

/*static*/ std::string FileHelper::cleanPath(const std::string& path)
{
	std::string fullPath = normalizePath(path);
	bool separatorEnded = StringHelper::endsWith(path, "\\");		// TODO: 不好的做法，要优化！

	if (!StringHelper::contains(fullPath, ":"))	// 如果不是绝对路径，拼接当前工作目录
		fullPath = currentPath() + "\\" + fullPath;

	std::vector<std::string> levels = StringHelper::split(fullPath, "\\", true);
	std::vector<std::string> newLevels;
	for (size_t i = 0; i < levels.size(); i++)
	{
		const std::string& level = levels.at(i);
		if (level == ".")
		{
		}
		else if (level == "..")
		{
			if (newLevels.size() > 1)	// 只剩1个层级，不允许再返回上一级了
				newLevels.pop_back();
		}
		else
		{
			newLevels.push_back(level);
		}
	}

	std::string newPath = StringHelper::join(newLevels, "\\");
	if (separatorEnded)
		newPath.append("\\");

	return newPath;
}

/*static*/ std::string FileHelper::fileAbsolutePath(const std::string& file)
{
	std::string newFile = normalizePath(file);
	size_t pos = newFile.rfind("\\");
	if (pos == std::string::npos)
		return "";

	return newFile.substr(0, pos);
}

/*static*/ std::string FileHelper::fileName(const std::string& file)
{
	std::string newFile = normalizePath(file);
	size_t pos = newFile.rfind("\\");
	if (pos == std::string::npos)
		return newFile;

	return newFile.substr(pos + 1);
}

/*static*/ std::string FileHelper::fileCompleteBaseName(const std::string& file)
{
	std::string fn = fileName(file);
	size_t pos = fn.rfind(".");
	if (pos == std::string::npos)
		return fn;

	return fn.substr(0, pos);
}

/*static*/ std::string FileHelper::fileSuffix(const std::string& file)
{
	std::string fn = fileName(file);
	size_t pos = fn.rfind(".");
	if (pos == std::string::npos)
		return "";

	return fn.substr(pos + 1);
}

/*static*/ __int64 FileHelper::fileSize(const std::string& file)
{
	LARGE_INTEGER fileSize;
	HANDLE handle = CreateFileA(file.c_str(), GENERIC_READ, 0, NULL, 
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	GetFileSizeEx(handle, &fileSize);
	CloseHandle(handle);

	return fileSize.QuadPart;
}

/*static*/ std::string FileHelper::prettySize(__int64 size)
{
	__int64 gb = (1 << 30);
	__int64	mb = (1 << 20);
	__int64 kb = (1 << 10);

	double value = 0.0;
	char buf[16];

	if (size >= gb)
	{
		value = 1.0 * size / gb;
		sprintf_s(buf, "%.2f GB", value);
	}
	else if (size < gb && size >= mb)
	{
		value = 1.0 * size / mb;
		sprintf_s(buf, "%.2f MB", value);
	}
	else if (size < mb && size >= kb)
	{
		value = 1.0 * size / kb;
		sprintf_s(buf, "%.2f KB", value);
	}
	else
	{
		sprintf_s(buf, "%d B", (int)size);
	}

	return std::string(buf);
}

/*static*/ std::string prettySize(__int64 size)
{
	__int64 gb = (1 << 30);
	__int64	mb = (1 << 20);
	__int64 kb = (1 << 10);

	double value = 0.0;
	char buf[16];

	if (size >= gb)
	{
		value = 1.0 * size / gb;
		sprintf_s(buf, "%.2f GB", value);
	}
	else if (size < gb && size >= mb)
	{
		value = 1.0 * size / mb;
		sprintf_s(buf, "%.2f MB", value);
	}
	else if (size < mb && size >= kb)
	{
		value = 1.0 * size / kb;
		sprintf_s(buf, "%.2f KB", value);
	}
	else
	{
		sprintf_s(buf, "%d B", (int)size);
	}

	return std::string(buf);
}

/*static*/ bool FileHelper::fileExists(const std::string& file)
{
	return (_access(file.c_str(), 0) == 0);
}

/*static*/ bool FileHelper::isFile(const std::string& target)
{
	return !(GetFileAttributesA(target.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
}

/*static*/ bool FileHelper::isDirectory(const std::string& target)
{
	return !!(GetFileAttributesA(target.c_str()) & FILE_ATTRIBUTE_DIRECTORY);
}

/*static*/ void FileHelper::removeFile(const std::string& file)
{
	if (!fileExists(file))
		return;

	std::remove(file.c_str());
}

/*static*/ void FileHelper::copyFile(const std::string& file, const std::string& newFile)
{
	if (!isFile(file))
		return;

	CopyFileA(file.c_str(), newFile.c_str(), FALSE);
}

/*static*/ void FileHelper::removeDir(const std::string& dir)
{
	SHFILEOPSTRUCTA fileOp;
	memset((void*)&fileOp, 0, sizeof(SHFILEOPSTRUCT));

	std::string cpDir = dir + '\0';

	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	fileOp.hNameMappings = NULL;
	fileOp.hwnd = NULL;
	fileOp.lpszProgressTitle = NULL;
	fileOp.pFrom = cpDir.c_str();
	fileOp.pTo = NULL;
	fileOp.wFunc = FO_DELETE;

	SHFileOperationA(&fileOp);
}

/*static*/ void FileHelper::copyDir(const std::string& dir, const std::string& newDir)
{
	SHFILEOPSTRUCTA fileOp;
	memset((void*)&fileOp, 0, sizeof(SHFILEOPSTRUCT));

	std::string cpDir = dir + '\0';
	std::string cpNewDir = newDir + '\0';

	fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	fileOp.hNameMappings = NULL;
	fileOp.hwnd = NULL;
	fileOp.lpszProgressTitle = NULL;
	fileOp.pFrom = cpDir.c_str();
	fileOp.pTo = cpNewDir.c_str();
	fileOp.wFunc = FO_COPY;

	SHFileOperationA(&fileOp);
}

/*static*/ void FileHelper::emptyDir(const std::string& dir)
{
	if (!isDirectory(dir))
		return;

	std::vector<std::string> subFiles, subDirs;
	entryDir(dir, subFiles, subDirs);
	for (size_t i = 0; i < subFiles.size(); i++)
		removeFile(subFiles[i]);

	for (size_t i = 0; i < subDirs.size(); i++)
		removeDir(subDirs[i]);
}

/*static*/ void FileHelper::createPath(const std::string& path)
{
	std::string clPath = cleanPath(path + "\\");

	char tmpDirPath[1024] = { 0 };
	for (size_t i = 0; i < clPath.size(); i++)
	{
		tmpDirPath[i] = clPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (_access(tmpDirPath, 0) == -1)
			{
				int ret = _mkdir(tmpDirPath);
				//if (ret == -1) return ret;
			}
		}
	}
}

/*static*/ void FileHelper::entryDir(const std::string& dir, std::vector<std::string>& subFiles, std::vector<std::string>& subDirs, bool recursive /*= false*/)
{
	std::stack<std::string> dirStack;
	dirStack.push(dir);

	// 深度优先遍历
	while (!dirStack.empty())
	{
		std::string topDir = dirStack.top();
		dirStack.pop();

		std::string findDir = topDir + "\\*";

		_finddata_t fd;
		intptr_t handle = _findfirst(findDir.c_str(), &fd);
		if (handle == -1)
			return;

		do
		{
			std::string fn = fd.name;
			if (fn == "." || fn == "..")
				continue;

			fn = topDir + "\\" + fd.name;

			if (fd.attrib & _A_SUBDIR)
			{
				subDirs.push_back(fn);
				if (recursive)
					dirStack.push(fn);
			}
			else
				subFiles.push_back(fn);
	
		} while (!_findnext(handle, &fd));
	}
}

/*static*/ std::string FileHelper::applicationPath()
{
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	std::string exeFullPath = buf;
	return fileAbsolutePath(exeFullPath);
}

/*static*/ std::string FileHelper::systemAppDataPath()
{
	// "C:\Users\admin\AppData\Roaming\"
	char buf[MAX_PATH];
	SHGetSpecialFolderPathA(NULL, buf, CSIDL_APPDATA, TRUE);
	std::string path = buf;
	path += "\\";

	return path;
}

/*static*/ std::string FileHelper::currentPath()
{
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);

	return std::string(buf);
}