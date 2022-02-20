#include "pch.h"
#include "FileHelpers.h"

std::string Tools::ReadWholeFile(const std::string& aFile)
{
	std::vector<char> buffer = {'\0'};
	std::ifstream infile(aFile,std::ios::ate | std::ios::in);
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

std::vector<char> Tools::ReadWholeFileBinary(const std::string& aFile)
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

bool Tools::FileExists(const std::string& aFile)
{
	return std::ifstream(aFile).operator bool();
}

time_t Tools::FileLastModified(const std::string& aFile)
{
	struct stat result;
	if (stat(aFile.c_str(), &result) == 0 /* S_OK */)
	{
		return result.st_mtime;
	}

	return -1;
}
