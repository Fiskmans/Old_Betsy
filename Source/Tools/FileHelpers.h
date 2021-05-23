#pragma once

namespace Tools
{
	std::string ReadWholeFile(const std::string& aFile);
	std::vector<char> ReadWholeFileBinary(const std::string& aFile);
	bool FileExists(const std::string& aFile);
	time_t FileLastModified(const std::string& aFile);
}