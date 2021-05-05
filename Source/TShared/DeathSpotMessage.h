#pragma once
#include "NetMessage.h"
#include "Vector.hpp"
class DeathSpotMessage :
	public NetMessage
{
public:
	DeathSpotMessage();

	enum Category : unsigned char
	{
		Invalid,
		Announcement,
		Request
	};

	Category myCategory;
	V3F myPosition;
};

