#pragma once

#define MAXPACKAGESIZE 576

typedef unsigned int NetMessageIdType;

class NetMessage
{
public:
	enum class Type : short
	{
		Invalid,
		Setup,
		Status,
		PackageResponse,
		BigFile,
		Identify,
		Ping,
		DoWork,
		Rapport,
		DeathMessage
	};
	NetMessage();
	Type  myType;
	NetMessageIdType myNetMessageID;
	bool IsImportant() const;
	char* begin();

	template<class T>
	operator T* ()
	{
		return reinterpret_cast<T*>(this);
	}

	static NetMessageIdType ourCounter;
};

class NetMessageImportantResponse : public NetMessage
{
public:
	size_t myMessageHash;
};

inline NetMessage::NetMessage()
{
	myNetMessageID = ourCounter++;
	myType = Type::Invalid;
}

inline char* NetMessage::begin()
{
	return reinterpret_cast<char*>(&myType);
}
