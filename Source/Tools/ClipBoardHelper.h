#pragma once
#include <string>
namespace Tools
{
	void WriteToClipboard(const std::string& aString);
	std::string ReadFromClipboard();
}
