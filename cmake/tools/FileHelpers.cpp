#include "tools/FileHelpers.h"

#include <fstream>

#define NOMINMAX
#include <windows.h>

namespace tools 
{
	std::string ReadWholeFile(const std::string& aFile)
	{
		std::vector<char> buffer = {'\0'};
		std::ifstream infile(aFile, std::ios::ate | std::ios::in);
		if (infile)
		{
			size_t size = infile.tellg();
			infile.seekg(std::ios::beg);
			buffer.resize(size+1);
			infile.read(buffer.data(), size);
			buffer.back() = '\0';
		}
		else
		{
			throw std::exception("Could not open file");
		}
		return buffer.data();
	}

	std::vector<char> ReadWholeFileBinary(const std::string& aFile)
	{
		std::vector<char> buffer;
		std::ifstream infile(aFile, std::ios::ate | std::ios::binary | std::ios::in);
		if (infile)
		{
			size_t size = infile.tellg();
			infile.seekg(std::ios::beg);
			buffer.resize(size);
			infile.read(buffer.data(), size);
		}
		else
		{
			throw std::exception("Could not open file");
		}
		return buffer;
	}

	bool FileExists(const std::string& aFile)
	{
		return std::ifstream(aFile).operator bool();
	}

	time_t FileLastModified(const std::string& aFile)
	{
		struct stat result;
		if (stat(aFile.c_str(), &result) == 0 /* S_OK */)
		{
			return result.st_mtime;
		}

		return -1;
	}
	std::string NormalizeSlashes(const std::string& aFilePath)
	{
		std::string out = aFilePath;

		for (char& c : out)
			if (c == '\\')
				c = '/';

		return out;
	}
}
