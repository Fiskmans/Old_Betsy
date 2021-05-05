#pragma once
#include <string>

typedef void CURL;

namespace Tools
{
	std::string SimpleAuthOp(CURL* aCurl, const std::string& aUrl, std::string& aCurrentToken);
}
