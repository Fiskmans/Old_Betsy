#pragma once
#include <string>

typedef void CommonUtilitiesRL;

namespace Tools
{
	std::string SimpleAuthOp(CommonUtilitiesRL* aCurl, const std::string& aUrl, std::string& aCurrentToken);
}
