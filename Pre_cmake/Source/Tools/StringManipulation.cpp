#include <pch.h>
#include "StringManipulation.h"
#include <filesystem>

namespace Tools
{
	std::string PathWithoutFile(const std::string& aFilePath)
	{
		return aFilePath.substr(0, aFilePath.length() - std::filesystem::path(aFilePath).filename().string().length());
	}
	std::string ExtensionFromPath(const std::string& aFilePath)
	{
		return std::filesystem::path(aFilePath).extension().string();
	}
	std::string PrintByteSize(size_t aAmount)
	{
		if (aAmount < 1_kb)
		{
			return std::to_string(aAmount) + "b";
		}
		else if(aAmount < 1_mb)
		{
			return std::to_string(aAmount/ 1_kb) + "kb";
		}
		else if (aAmount < 1_gb)
		{
			return std::to_string(aAmount / 1_mb) + "mb";
		}
		else if (aAmount < 1_tb)
		{
			return std::to_string(aAmount / 1_gb) + "gb";
		}
		else
		{
			return std::to_string(aAmount / 1_tb) + "tb";
		}
	}
}