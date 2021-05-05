#pragma once
#include "NetMessage.h"
#include <bitset>
#include <string>
#include <vector>


#define CHUNKSIZE 512L
#define CHUNKSPERSEGMENT 128L
#define BYTESPERSEGMENT (CHUNKSPERSEGMENT * CHUNKSIZE)
#define NAMELENGTH 128
class NetworkInterface;

namespace BigFileStructs
{

	struct Head
	{
		size_t myChecksum;
		size_t myDataSize;
		char myName[NAMELENGTH];
	};
	struct SegmentDeclaration
	{
		size_t myChecksum;

	};
	struct SegmentResponse
	{
		std::bitset<CHUNKSPERSEGMENT> myMissingPackets;
	};
	struct Chunk
	{
		size_t myChecksum;
		unsigned char myIndex;
		char myData[CHUNKSIZE];
	};
}

class BigFileMessage : public NetMessage
{
public:
	BigFileMessage();
	enum class BigFileStage : char
	{
		Invalid,
		Head,
		SegmentDeclaration,
		SegmentResponse,
		Chunk,
		Done,
		Redo
	} myStage;

	union
	{
		BigFileStructs::Head myHead;
		BigFileStructs::SegmentDeclaration myDeclaration;
		BigFileStructs::SegmentResponse myResponse;
		BigFileStructs::Chunk myChunk;
	};
};

class BigFile
{
	struct Segment
	{
		size_t myChecksum;
		char* myBegin;
		std::bitset<CHUNKSPERSEGMENT> myChunksLeft;
	};
public:
	BigFile(const std::string& aData);
	BigFile(const char* aData, unsigned long aDataSize);
	BigFile(const BigFileStructs::Head& aHead);

	~BigFile();

	void Accept(const BigFileStructs::Chunk& aChunk);
	void Accept(const BigFileStructs::SegmentDeclaration& aSegmentHeader);
	void Accept(const BigFileStructs::SegmentResponse& aSegmentResponse);
	void Accept(const BigFileMessage* aMessage);

	void SetInterface(NetworkInterface* aInterface);
	void SetName(const std::string& aName);
	std::string GetName();

	void Start();

	bool Flush();

	std::vector<char> GetData();
	std::pair<const char*, size_t> AccessData();
private:
	
	void PeerAcceptedSegmentHeader(NetMessageIdType aId);
	void PeerAcceptedHead(NetMessageIdType aId);

	void SendNextSegmentHeader();
	void SendSegmentResponse();

	void SetupForSending();
	void SendMissingChunks();

	size_t AllocationSize(size_t aDataSize);
	void GenerateChecksum();
	
	NetworkInterface* myInterFaceToWorkAgainst;

	bool myIsSending;
	bool myHasSentHead;
	bool myIsComplete;

	float myLastUpdate;

	std::hash<std::string> myHasher;
	std::string myName;
	size_t myChecksum;
	Segment myCurrentSegment;

	int mySegmentCounter;
	int mySegmentCount;
	char myChunksLeftInTransit;

	size_t myDataSize;
	char* myData = nullptr;
};


