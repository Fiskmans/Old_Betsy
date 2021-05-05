#include <pch.h>
#include "BigFileMessage.h"
#include "NetworkInterface.h"
#include <iostream>
#include "TimeHelper.h"
#include <sstream>

BigFile::BigFile(const std::string& aData)
{
	myData = (char*)realloc(myData, AllocationSize(aData.size()));
	memcpy(myData, aData.data(), aData.size());
	myDataSize = aData.size();
	mySegmentCounter = -1;
	SetupForSending();
}

BigFile::BigFile(const char* aData, unsigned long aDataSize)
{
	myData = (char*)realloc(myData, AllocationSize(aDataSize));
	memcpy(myData, aData, aDataSize);
	myDataSize = aDataSize;
	mySegmentCounter = -1;
	SetupForSending();
}

BigFile::BigFile(const BigFileStructs::Head& aHead)
{
	myChecksum = aHead.myChecksum;
	myIsSending = false;
	myName = aHead.myName;
	mySegmentCounter = -1;
	myDataSize = aHead.myDataSize;
	myData = (char*)realloc(myData, AllocationSize(myDataSize));
	mySegmentCount = ceil(myDataSize / BYTESPERSEGMENT);
	std::stringstream ss;
	ss << std::hex << aHead.myChecksum;
	std::cout << "Recieving file named " + std::string(aHead.myName) + " of [" + std::to_string(aHead.myDataSize) + "] bytes and checksum: " + ss.str() + "\n";
}

BigFile::~BigFile()
{
	if (myData)
	{
		free(myData);
		myData = nullptr;
	}
}

void BigFile::Accept(const BigFileStructs::Chunk& aChunk)
{
	if (aChunk.myChecksum == myHasher(std::string(aChunk.myData, CHUNKSIZE)))
	{
		myCurrentSegment.myChunksLeft.reset(aChunk.myIndex);
		memcpy(myCurrentSegment.myBegin + CHUNKSIZE * aChunk.myIndex, aChunk.myData, CHUNKSIZE);
	}
	else
	{
		std::stringstream ss1;
		std::stringstream ss2;
		ss1 << std::hex << aChunk.myChecksum;
		ss2 << std::hex << myHasher(std::string(aChunk.myData, CHUNKSIZE));


		std::cout << "Chunk [" + std::to_string(aChunk.myIndex) + "] rejected cheacksum: " + ss1.str() + " did not match " + ss2.str() + "\n";
	}

	--myChunksLeftInTransit;
	if (myChunksLeftInTransit == 0)
	{
		std::cout << "All chunks recieved sending response\n";
		SendSegmentResponse();
		if (myCurrentSegment.myChunksLeft.none() && mySegmentCounter == mySegmentCount)
		{
			std::cout << "All segments recieved ending transmission\n";
			myIsComplete = true;
		}
	}
}

void BigFile::Accept(const BigFileStructs::SegmentDeclaration& aSegmentHeader)
{
	std::cout << "Recieving header\n";
	mySegmentCounter++;
	myCurrentSegment.myBegin = myData + mySegmentCounter * CHUNKSIZE * CHUNKSPERSEGMENT;
	myCurrentSegment.myChecksum = aSegmentHeader.myChecksum;
	myCurrentSegment.myChunksLeft.set();
	myChunksLeftInTransit = CHUNKSPERSEGMENT;
	myChunksLeftInTransit = myCurrentSegment.myChunksLeft.count();
}

void BigFile::Accept(const BigFileStructs::SegmentResponse& aSegmentResponse)
{
	myCurrentSegment.myChunksLeft = aSegmentResponse.myMissingPackets;
	std::string status;
	for (size_t i = 0; i < CHUNKSPERSEGMENT; i++)
	{
		status += myCurrentSegment.myChunksLeft[i] ? "1" : "0";
	}
	std::cout << "accepting segmentheader respnse: " + status + "\n";
	if (myCurrentSegment.myChunksLeft.any())
	{
		SendMissingChunks();
	}
	else
	{
		SendNextSegmentHeader();
	}
}

void BigFile::Accept(const BigFileMessage* aMessage)
{
	switch (aMessage->myStage)
	{
	case BigFileMessage::BigFileStage::SegmentDeclaration:
		Accept(aMessage->myDeclaration);
		break;
	case BigFileMessage::BigFileStage::Chunk:
		Accept(aMessage->myChunk);
		break;
	case BigFileMessage::BigFileStage::SegmentResponse:
		Accept(aMessage->myResponse);
		break;
	default:
		break;
	}
}

void BigFile::SetInterface(NetworkInterface* aInterface)
{
	myInterFaceToWorkAgainst = aInterface;
}

void BigFile::SetName(const std::string& aName)
{
	myName = aName;
}

std::string BigFile::GetName()
{
	return myName;
}

void BigFile::Start()
{
	std::cout << "Sending File header [" + std::to_string(myDataSize) + "] bytes in [" + std::to_string(mySegmentCount) + "] segments and [" + std::to_string(AllocationSize(myDataSize) / CHUNKSIZE) + "] chunks\n";
	BigFileMessage mess;
	mess.myStage = BigFileMessage::BigFileStage::Head;
	mess.myHead.myChecksum = myChecksum;
	mess.myHead.myDataSize = myDataSize;
	strcpy_s<NAMELENGTH>(mess.myHead.myName, myName.c_str());
	myHasSentHead = true;
	myInterFaceToWorkAgainst->HookCallBack(mess.myNetMessageID, std::bind(&BigFile::PeerAcceptedHead, this, std::placeholders::_1));
	myInterFaceToWorkAgainst->PreProcessAndSend(&mess, sizeof(mess));
}

bool BigFile::Flush()
{
	if (myIsComplete)
	{
		return true;
	}

	if (!myInterFaceToWorkAgainst)
	{
		throw std::exception("No interface, call SetInterface if you want this class to transmit data");
	}
	return false;
}

std::vector<char> BigFile::GetData()
{
	std::vector<char> out;
	out.reserve(myDataSize);
	for (size_t i = 0; i < myDataSize; i++)
	{
		out.push_back(*(myData + i));
	}
	return out;
}

std::pair<const char*, size_t> BigFile::AccessData()
{
	return std::pair<const char*, size_t>(myData,myDataSize);
}

void BigFile::SetupForSending()
{
	mySegmentCount = ceil(double(myDataSize) / BYTESPERSEGMENT);
	GenerateChecksum();
	myIsSending = true;
	myName = "Unnamed";
	myHasSentHead = false;
}

void BigFile::SendMissingChunks()
{
	for (size_t i = 0; i < CHUNKSPERSEGMENT; i++)
	{
		if (myCurrentSegment.myChunksLeft[i])
		{
			//std::cout << "(re)sending chunk [" + std::to_string(i) + "]\n";
			BigFileMessage mess;
			mess.myStage = BigFileMessage::BigFileStage::Chunk;
			mess.myChunk.myIndex = i;
			memcpy(mess.myChunk.myData, myCurrentSegment.myBegin + i * CHUNKSIZE, CHUNKSIZE);
			mess.myChunk.myChecksum = myHasher(std::string(mess.myChunk.myData, CHUNKSIZE));
			myInterFaceToWorkAgainst->PreProcessAndSend(&mess, sizeof(mess));
		}
	}
}

size_t BigFile::AllocationSize(size_t aDataSize)
{
	return ceil(double(aDataSize) / BYTESPERSEGMENT) * BYTESPERSEGMENT;
}

void BigFile::GenerateChecksum()
{
	myChecksum = myHasher(std::string(myData, myDataSize));
}

BigFileMessage::BigFileMessage() : NetMessage()
{
	myType = NetMessage::Type::BigFile;
	myStage = BigFileStage::Invalid;
}

void BigFile::PeerAcceptedSegmentHeader(NetMessageIdType aId)
{
	SendMissingChunks();
}

void BigFile::PeerAcceptedHead(NetMessageIdType aID)
{
	SendNextSegmentHeader();
}

void BigFile::SendNextSegmentHeader()
{
	++mySegmentCounter;
	if (mySegmentCounter == mySegmentCount)
	{
		myIsComplete = true;
		return;
	}
	myCurrentSegment.myBegin = myData + BYTESPERSEGMENT * mySegmentCounter;
	myCurrentSegment.myChunksLeft.set();
	myCurrentSegment.myChecksum = myHasher(std::string(myCurrentSegment.myBegin, BYTESPERSEGMENT));

	std::stringstream ss;
	ss << std::hex << myCurrentSegment.myChecksum;
	std::cout << "Sending segmentdecl for number [" + std::to_string(mySegmentCounter+1) + "] of [" + std::to_string(mySegmentCount) + "] with checksum: " + ss.str() + "\n";

	BigFileMessage mess;
	mess.myStage = BigFileMessage::BigFileStage::SegmentDeclaration;
	mess.myDeclaration.myChecksum = myCurrentSegment.myChecksum;
	myInterFaceToWorkAgainst->HookCallBack(mess.myNetMessageID, std::bind(&BigFile::PeerAcceptedSegmentHeader, this, std::placeholders::_1));
	myInterFaceToWorkAgainst->PreProcessAndSend(&mess, sizeof(mess));
}

void BigFile::SendSegmentResponse()
{
	if (myCurrentSegment.myChecksum != myHasher(std::string(myCurrentSegment.myBegin,BYTESPERSEGMENT)))
	{
		myCurrentSegment.myChunksLeft.set();
	}
	std::string status;
	for (size_t i = 0; i < CHUNKSPERSEGMENT; i++)
	{
		status += myCurrentSegment.myChunksLeft[i] ? "1" : "0";
	}
	std::cout << "Sending segmentheader respnse: " + status + "\n";
	BigFileMessage mess;
	mess.myStage = BigFileMessage::BigFileStage::SegmentResponse;
	mess.myResponse.myMissingPackets = myCurrentSegment.myChunksLeft;
	myInterFaceToWorkAgainst->PreProcessAndSend(&mess, sizeof(mess));
	myChunksLeftInTransit = myCurrentSegment.myChunksLeft.count();
}
