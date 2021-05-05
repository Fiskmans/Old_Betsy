#pragma once
#include <EasyAssignString.h>
#include "Vector.hpp"

#ifdef __INTELLISENSE__
#include <pch.h>
#endif

struct DeathMarker
{
	EasyAssignString<MAXUSERNAMELENGTH> myName;
	V3F myPosition;
};

