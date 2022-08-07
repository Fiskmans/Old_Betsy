#ifndef ENGINE_TOOLS_FILE_HELPERS_H
#define ENGINE_TOOLS_FILE_HELPERS_H

#include <string>
#include <vector>
#include <ctime>

namespace tools
{
	std::string ReadWholeFile(const std::string& aFile);
	std::vector<char> ReadWholeFileBinary(const std::string& aFile);
	bool FileExists(const std::string& aFile);
	time_t FileLastModified(const std::string& aFile);
	std::string NormalizeSlashes(const std::string& aFilePath);
}

#endif