#pragma once
#include <string>

template<size_t SIZE>
class EasyAssignString
{
public:

	EasyAssignString& operator=(const std::string& aString)
	{
		WIPE(myBuffer);
		memcpy(myBuffer, aString.c_str(), MIN(aString.size(), SIZE-1));
		return *this;
	}

	operator std::string() 
	{
		return myBuffer;
	}

	operator char* ()
	{
		return myBuffer;
	}

	operator const std::string() const
	{
		return myBuffer;
	}

	operator const char* () const
	{
		return myBuffer;
	}


private:
	char myBuffer[SIZE];
};
template<size_t SIZE>
std::string operator+(const std::string& aLhs, const EasyAssignString<SIZE> aRhs)
{
	return aLhs + (std::string)aRhs;
}

