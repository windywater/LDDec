#pragma once
#include <string>
#include <vector>

class FileHelper
{
public:
	static std::string readFromFile(const std::string& fileName, unsigned int size = -1);
	static void writeToFile(const std::string& fileName, const std::string& content);

	static std::string normalizePath(const std::string& file);
	static std::string cleanPath(const std::string& path);
	static std::string fileAbsolutePath(const std::string& file);
	static std::string fileName(const std::string& file);
	static std::string fileCompleteBaseName(const std::string& file);
	static std::string fileSuffix(const std::string& file);
	static __int64 fileSize(const std::string& file);
	static std::string prettySize(__int64 size);

	static bool fileExists(const std::string& file);
	static bool isFile(const std::string& target);
	static bool isDirectory(const std::string& target);
	static void removeFile(const std::string& file);
	static void copyFile(const std::string& file, const std::string& newFile);
	static void removeDir(const std::string& dir);
	static void copyDir(const std::string& dir, const std::string& newDir);
	static void emptyDir(const std::string& dir);	// 清空目录下面的文件和目录，保留此目录存在

	static void entryDir(const std::string& dir, std::vector<std::string>& subFiles, std::vector<std::string>& subDirs, bool recursive = false);

	static void createPath(const std::string& path);
	static std::string applicationPath();
	static std::string systemAppDataPath();
	static std::string currentPath();
};

