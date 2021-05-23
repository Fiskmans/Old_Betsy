#include <pch.h>
#include "StringManipulation.h"
#include <filesystem>

namespace Tools
{
	std::string PathWithoutFile(const std::string& aFilePath)
	{
		return aFilePath.substr(0, aFilePath.length() - std::filesystem::path(aFilePath).filename().string().length());
	}
}