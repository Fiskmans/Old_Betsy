#include <pch.h>
#include "FiskWebSocketConnection.h"
#include <bcrypt.h>
#include "SSLHelpers.h"
#include "Logger.h"
#include <thread>
#include <openssl/ssl.h>
#include <functional>

namespace Tools
{

	void ReverseBytes(char* aData, size_t aSize)
	{
		for (size_t i = 0; i < aSize / 2; i++)
		{
			std::swap(aData[i], aData[aSize - i - 1]);
		}
	}

	void FiskWebSocketConnection::Write(const std::string& aData)
	{
		SYSNETWORK("Sending: [" + aData + "]");
		const size_t largestBlockSize = 0xFF;
		static_assert(largestBlockSize < 0xFFFF, "You need to implement 4 byte length extension for this to work");
		char transmissionBuffer[largestBlockSize + 2 /*flags*/ + 2 /*potetial length extension*/ + 4 /*masking key*/];

		size_t packetCount = 0;

		unsigned char maskingkey[4];
		static char* data = nullptr;


		char flags[2];
		ZeroMemory(flags, 2);
		flags[0] |= OpCodes::Text;
		flags[1] |= Flags::Mask;


		size_t left = aData.length();
		size_t at = 0;
		data = new char[left];
		if (!data)
		{
			throw std::bad_alloc();
		}
		memcpy(data, aData.c_str(), left);
		while (left > 0)
		{
			if (flags[1] & Flags::Mask)
			{
				BCryptGenRandom(myAlgHandle, maskingkey, 4, 0);
			}
			at = 0;
			size_t transmiting = left;
			if (left > largestBlockSize)
			{
				transmiting = largestBlockSize;
			}
			if (transmiting == left)
			{
				flags[0] |= Flags::Fin;
			}


			flags[1] &= ~Flags::PayloadLength;
			if (transmiting < 126)
			{
				flags[1] |= Flags::PayloadLength & transmiting;
			}
			else if (transmiting <= 0xFFFF)
			{
				flags[1] |= Flags::PayloadLength & unsigned char(127);
			}
			memcpy(transmissionBuffer, flags, 2);
			at += 2;
			if (transmiting >= 126)
			{
				memcpy(transmissionBuffer + at, &transmiting, 2);
				at += 2;
			}
			if (flags[1] & Flags::Mask)
			{
				memcpy(transmissionBuffer + at, &maskingkey, 4);
				at += 4;
				MaskData(data, transmiting, reinterpret_cast<char*>(maskingkey));
			}

			memcpy(transmissionBuffer + at, data, transmiting);
			at += transmiting;
			SSL::SendBytes(myConnection, transmissionBuffer, at);

			left -= transmiting;
			data += left;
			packetCount++;
			flags[0] &= ~OpCodes::Text;
			flags[0] |= OpCodes::Continuation;
		}
		delete[] data;
	}

	std::string FiskWebSocketConnection::GetTextPackage()
	{
		std::lock_guard guard(myMutex);
		std::string ret = myTextPacketQueue.front();
		myTextPacketQueue.pop();
		return ret;
	}

	std::string FiskWebSocketConnection::GetBinaryPackage()
	{
		std::lock_guard guard(myMutex);
		std::string ret = myTextPacketQueue.front();
		myTextPacketQueue.pop();
		return ret;
	}

	std::string FiskWebSocketConnection::WaitForTextPackage()
	{
		while (!HasTextPackage())
		{
			std::this_thread::yield();
		}
		return GetTextPackage();
	}
	std::string FiskWebSocketConnection::WaitForBinaryPackage()
	{
		while (!HasBinaryPackage())
		{
			std::this_thread::yield();
		}
		return GetBinaryPackage();
	}

	bool FiskWebSocketConnection::HasTextPackage()
	{
		return !myTextPacketQueue.empty();
	}
	bool FiskWebSocketConnection::HasBinaryPackage()
	{
		return !myBinaryPacketQueue.empty();
	}
	void FiskWebSocketConnection::CloseConnection()
	{
		char flags[2];
		ZeroMemory(flags, 2);
		flags[0] |= OpCodes::ConnectionClose;
		flags[0] |= Flags::Fin;
		SSL::SendBytes(myConnection, flags, 2);
		closesocket(SSL_get_fd(myConnection));
	}
	FiskWebSocketConnection::FiskWebSocketConnection(::SSL* aConnection)
	{
		myConnection = aConnection;
		myFlags[0] = '\0';
		myFlags[1] = '\0';
		BCryptOpenAlgorithmProvider(&myAlgHandle, BCRYPT_RNG_ALGORITHM, NULL, 0);
		myReadThread = std::thread(&FiskWebSocketConnection::Read,this);
	}
	FiskWebSocketConnection::~FiskWebSocketConnection()
	{
		BCryptCloseAlgorithmProvider(myAlgHandle, 0);
		CloseConnection();
	}
	void FiskWebSocketConnection::Read()
	{
		NAMETHREAD(L"Websocket-reader");
		while (myConnection)
		{
			char flags[2];
			int result = SSL::RecvBytes(myConnection, flags, 2);


			bool Final = !!(flags[0] & Flags::Fin);
			bool Rsv1 = !!(flags[0] & Flags::Rsv1);
			bool Rsv2 = !!(flags[0] & Flags::Rsv2);
			bool Rsv3 = !!(flags[0] & Flags::Rsv3);
			char OpCode = (flags[0] & Flags::OpCode);
			bool IsMasked = !!(flags[1] & Flags::Mask);
			size_t PayloadLength = (flags[1] & Flags::PayloadLength);

			if (OpCode == OpCodes::Ping || OpCode == OpCodes::Pong)
			{
				return;
			}



			if (!Final && OpCode != OpCodes::Continuation)
			{
				memcpy(myFlags, flags, 2);
			}

			char shortPayload[126 + 1];
			char* largePayload = nullptr;

			if (PayloadLength > 0)
			{
				if (PayloadLength < 126)
				{
					SSL::RecvBytes(myConnection, shortPayload, PayloadLength);
				}
				else
				{
					if (PayloadLength == 126)
					{
						//std::cout << "Reading 2 bytes of size: " << PayloadLength << std::endl;
						SSL::RecvBytes(myConnection, reinterpret_cast<char*>(&PayloadLength), 2);
						ReverseBytes(reinterpret_cast<char*>(&PayloadLength), 2);
					}
					else
					{
						//std::cout << "Reading 8 bytes of size: " << PayloadLength << std::endl;
						SSL::RecvBytes(myConnection, reinterpret_cast<char*>(&PayloadLength), 8);
						ReverseBytes(reinterpret_cast<char*>(&PayloadLength), 8);
					}
					//PrintBits(reinterpret_cast<char*>(&PayloadLength), 8);
					//std::cout << std::endl;
					largePayload = new char[PayloadLength + 1];
				}
			}

			char maskkey[4];
			if (IsMasked)
			{
				SSL::RecvBytes(myConnection, maskkey, 4);
			}

			if (largePayload)
			{
				SSL::RecvBytes(myConnection, largePayload, PayloadLength);
			}

			if (IsMasked)
			{
				if (largePayload)
				{
					MaskData(largePayload, PayloadLength, maskkey);
				}
				else
				{
					MaskData(shortPayload, PayloadLength, maskkey);
				}
			}

			//std::cout << "Final: " << Final << std::endl;
			//std::cout << "Rsv1: " << Rsv1 << std::endl;
			//std::cout << "Rsv2: " << Rsv2 << std::endl;
			//std::cout << "Rsv3: " << Rsv3 << std::endl;
			//std::cout << "OpCode: " << OpcodeToString(OpCode) << std::endl;
			//std::cout << "Mask: " << IsMasked << std::endl;
			//std::cout << "payloadLength: " << int(PayloadLength) << std::endl;
			if (PayloadLength > 0 && PayloadLength < 126)
			{
				shortPayload[PayloadLength] = '\0';
				SYSNETWORK("payload: [" + std::string(shortPayload) + "]");
			}
			if (OpCode == OpCodes::ConnectionClose)
			{
				SYSNETWORK("Closing connection");
				CloseConnection();
				return;
			}
			if (largePayload)
			{
				ReadBuffer.write(largePayload, PayloadLength);
			}
			else
			{
				ReadBuffer.write(shortPayload, PayloadLength);
			}

			if (Final)
			{
				if (OpCode == OpCodes::Binary || ((myFlags[0] & Flags::OpCode) == OpCodes::Binary))
				{
					if (PayloadLength > 0)
					{
						myBinaryPacketQueue.emplace(ReadBuffer.str());
					}
				}
				else if (OpCode == OpCodes::Text || ((myFlags[0] & Flags::OpCode) == OpCodes::Text))
				{
					if (PayloadLength > 0)
					{
						myTextPacketQueue.emplace(ReadBuffer.str());
					}
				}
				ReadBuffer.str("");
				ReadBuffer.clear();
			}
		}
	}
	void FiskWebSocketConnection::MaskData(char* aData, size_t aSize, char* aMaskingKey)
	{
		for (size_t index = 0; index < aSize; index++)
		{
			aData[index] ^= aMaskingKey[index % 4];
		}
	}
	std::string FiskWebSocketConnection::OpcodeToString(char aCode)
	{
		switch (aCode)
		{
		case OpCodes::Continuation:
			return "Continuation";
		case OpCodes::Text:
			return "Text";
		case OpCodes::Binary:
			return "Binary";
		case OpCodes::ConnectionClose:
			return "Close Connection";
		case OpCodes::Ping:
			return "Ping";
		case OpCodes::Pong:
			return "Pong";
		default:
			return "Unkown OpCode" + std::to_string(aCode);
		}
	}
}