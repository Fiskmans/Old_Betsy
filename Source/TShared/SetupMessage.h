#pragma once
#include "NetMessage.h"
#include <string>
#include "EasyAssignString.h"

#ifdef __INTELLISENSE__
#include <pch.h>
#endif 

#define MAXIDENTIFIERLENGTH 128

class SetupMessage :
	public NetMessage
{
public:
	SetupMessage();
	enum class SetupStep : char
	{
		Invalid,
		Request,
		Response
	};

	SetupStep myStep;
	union
	{
		EasyAssignString<MAXIDENTIFIERLENGTH> myIdentifier;
		struct
		{
			bool myResult;
			unsigned short myID;
		} myResponse;
	};

	void SetIdentifier(const std::string& aIdentifier);
};

