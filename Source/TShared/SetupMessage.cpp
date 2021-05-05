#include <pch.h>
#include "SetupMessage.h"

SetupMessage::SetupMessage()
{
	myType = Type::Setup;
}

void SetupMessage::SetIdentifier(const std::string& aIdentifier)
{
	memset(myIdentifier, '\0', MAXIDENTIFIERLENGTH);
	memcpy(myIdentifier, aIdentifier.c_str(), MAXIDENTIFIERLENGTH - 1);
}
