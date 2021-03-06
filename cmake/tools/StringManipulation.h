#ifndef TOOLS_STRING_MANIPULATION_H
#define TOOLS_STRING_MANIPULATION_H

#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>

namespace tools
{

	std::string PathWithoutFile(const std::string& aFilePath);
	std::string ExtensionFromPath(const std::string& aFilePath);
	std::string PrintByteSize(size_t aAmount);

	// trim from start (in place)
	static inline void ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
			}));
	}

	// trim from end (in place)
	static inline void rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
			}).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string& s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}
}

#endif