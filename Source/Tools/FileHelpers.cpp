#include "pch.h"
#include "FileHelpers.h"

std::string Tools::ReadWholeFile(std::string aFile)
{
	std::vector<char> buffer = {'\0'};
	std::ifstream infile(aFile,std::ios::ate | std::ios::in);
	if (infile)
	{
		size_t size = infile.tellg();
		infile.seekg(std::ios::beg);
		buffer.resize(size);
		infile.read(buffer.data(), size);
		buffer.push_back('\0');
	}
	else
	{
		throw std::exception("Could not open file");
	}
	return buffer.data();
}

bool Tools::FileExists(std::string aFile)
{
	return std::ifstream(aFile).operator bool();
}
