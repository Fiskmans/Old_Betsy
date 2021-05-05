#pragma once
#include "PostMaster.hpp"
class Publisher
{
public:
	Publisher() = default;
	~Publisher() = default;

protected:
	void SendMessages(const Message& aMessage);
	void SendMessages(MessageType aMessage);
};