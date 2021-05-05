#include "pch.h"
#include "PingMessage.h"

PingMessage::PingMessage()
{
	myType = NetMessage::Type::Ping;
}
