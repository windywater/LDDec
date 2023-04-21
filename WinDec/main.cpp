#include <iostream>
#include "FileHelper.h"
#include "StringHelper.h"
#include <Windows.h>

static const std::string kEncryptedTag = "\x88\x7d\x1c";
static bool isEncryptedFile(const std::string& file)
{
	if (FileHelper::fileSize(file) < 3)
		return false;

	std::string headBytes = FileHelper::readFromFile(file, 3);
	if (headBytes == kEncryptedTag)
		return true;

	return false;
}

static bool fileTimes(const std::string& file, FILETIME& create, FILETIME& access, FILETIME& modified)
{
	HANDLE handle = CreateFile(file.c_str(),
		FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	BOOL ok = GetFileTime(handle, &create, &access, &modified);
	CloseHandle(handle);

	return (ok == TRUE);
}

static bool setFileTimes(const std::string& file, const FILETIME& create, const FILETIME& access, const FILETIME& modified)
{
	HANDLE handle = CreateFile(file.c_str(),
		FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
		return false;

	BOOL ok = SetFileTime(handle, &create, &access, &modified);
	CloseHandle(handle);

	return (ok == TRUE);
}

static bool decFile(const std::string& file)
{
	if (!isEncryptedFile(file))
		return false;

	FILETIME create, access, modified;
	fileTimes(file, create, access, modified);

	std::string tempFile = file + "_dec";
	std::string cmd = "type \"" + file + "\" > \"" + tempFile + "\"";
	system(cmd.c_str());
	FileHelper::removeFile(file);
	std::rename(tempFile.c_str(), file.c_str());

	setFileTimes(file, create, access, modified);

	return true;
}

enum Mode
{
	Decrypt,
	View
};

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "dec (file or directory)" << std::endl;
		std::cout << "dec (file or directory) -view" << std::endl;
		return -1;
	}

	Mode mode = Decrypt;

	std::string fileOrDir = argv[1];
	if (!FileHelper::fileExists(fileOrDir))
	{
		std::cout << "File or dir not exists!" << std::endl;
		return -2;
	}

	if (argc > 2)
	{
		if (StringHelper::toLower(argv[2]) == "-view")
			mode = View;
	}

	if (mode == Decrypt)
	{
		if (FileHelper::isFile(fileOrDir))
			decFile(fileOrDir);
		else
		{
			int count = 0;
			std::vector<std::string> subFiles, subDirs;
			FileHelper::entryDir(fileOrDir, subFiles, subDirs, true);
			for (size_t i = 0; i < subFiles.size(); i++)
			{
				if (decFile(subFiles.at(i)))
				{
					std::cout << subFiles.at(i) << std::endl;
					count++;
				}
			}

			std::cout << count << " file(s) decrypted." << std::endl;
		}
	}
	else if (mode == View)
	{
		if (FileHelper::isFile(fileOrDir))
		{
			if (isEncryptedFile(fileOrDir))
				std::cout << "File encrypted." << std::endl;
			else
				std::cout << "File NOT encrypted." << std::endl;
		}
		else
		{
			std::vector<std::string> subFiles, subDirs;
			FileHelper::entryDir(fileOrDir, subFiles, subDirs, true);
			std::cout << "Encrypted files:" << std::endl;
			for (size_t i = 0; i < subFiles.size(); i++)
			{
				const std::string& subFile = subFiles.at(i);
				if (isEncryptedFile(subFile))
					std::cout << subFile << std::endl;
			}
		}
	}

	return 0;
}