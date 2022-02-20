#include <pch.h>
#include "WebSocketHandshakeEncoder.h"
#include <sstream>
#include <bcrypt.h>
#include <algorithm>
#include <openssl/sha.h>
#include "Base64.h"

namespace Tools
{

	WebSocketHandshakeEncoder::WebSocketHandshakeEncoder()
	{
		BCryptOpenAlgorithmProvider(&myAlgHandle, BCRYPT_RNG_ALGORITHM, NULL, 0);
		mySetValues.reset();
	}

	WebSocketHandshakeEncoder::~WebSocketHandshakeEncoder()
	{
		BCryptCloseAlgorithmProvider(myAlgHandle, 0);
	}

	bool WebSocketHandshakeEncoder::ValidateResponse(const std::string& aResponseHeader)
	{
		const char* searchTerm = "sec-websocket-accept: ";
		std::cout << "Excpected key [" << myAcceptKey << "]" << std::endl;

		std::string lowerCase = aResponseHeader;
		std::transform(lowerCase.begin(), lowerCase.end(), lowerCase.begin(), [](unsigned char c) { return std::tolower(c); });
		size_t location = lowerCase.find(searchTerm);
		if (location == std::string::npos)
		{
			std::cout << "Could not find: [" << searchTerm << "] in header response" << std::endl;
			return false;
		}
		std::stringstream keyAndRest(aResponseHeader.substr(location + strlen(searchTerm)));
		std::string key;
		keyAndRest >> key;
		std::cout << "Exctracted key: [" << key << "]" << std::endl;
		return key == myAcceptKey;
	}

	std::string WebSocketHandshakeEncoder::Generate()
	{
		GenerateNonce();
		if (!mySetValues.all())
		{
			throw Not_Ready("This Handshake request is missing required fields");
		}
		std::stringstream stream;
		stream << "GET " << mySubHost << " HTTP/1.1\n";
		if (myUserAgent.length() > 0)
		{
			stream << "User-Agent " << myUserAgent << "\n";
		}
		stream << "Upgrade: websocket\n";
		stream << "Connection: Upgrade\n";
		stream << "Host: " << myHost << "\n";
		stream << "Sec-WebSocket-Key: " << myKey << "\n";
		stream << "Sec-WebSocket-Version: " << myVersion << "\n";
		stream << "\r\n";
		return stream.str();;
	}

	std::string WebSocketHandshakeEncoder::operator()()
	{
		return Generate();
	}

	WebSocketHandshakeEncoder::operator std::string()
	{
		return Generate();
	}

	WebSocketHandshakeEncoder& WebSocketHandshakeEncoder::SetUserAgent(const std::string& aValue)
	{
		myUserAgent = aValue;
		return *this;
	}

	WebSocketHandshakeEncoder& WebSocketHandshakeEncoder::SetHost(const std::string& aHost)
	{
		size_t divider = aHost.find('/');
		if (divider == std::string::npos)
		{
			throw std::invalid_argument("Hostname does not contain a subhost");
		}
		mySetValues[static_cast<int>(RequiredFields::Host)] = true;
		myHost = aHost.substr(0, divider);

		mySetValues[static_cast<int>(RequiredFields::SubHost)] = true;
		mySubHost = aHost.substr(divider);

		return *this;
	}

	WebSocketHandshakeEncoder& WebSocketHandshakeEncoder::SetVersion(unsigned short aVersion)
	{
		mySetValues[static_cast<int>(RequiredFields::Version)] = true;
		myVersion = aVersion;
		return *this;
	}

	void WebSocketHandshakeEncoder::GenerateNonce()
	{
		myKey == "";
		UCHAR* buffer = new UCHAR[16];
		BCryptGenRandom(myAlgHandle, buffer, 16, 0);
		size_t at = 0;
		while (at < 16)
		{
			if (buffer[at] > 128 || buffer[at] < 32)
			{
				BCryptGenRandom(myAlgHandle, buffer + at, 16 - at, 0);
			}
			else
			{
				at++;
			}
		}
		myKey = base64_encode(buffer, 16);

		const char* ServerGlobalGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; // this is a literal global variable, as in it's globally used around the globe
		const int preHashSize = myKey.length() + 36;

		unsigned char preHashbuffer[128]; // larger than 36 + myKey.length();
		static_assert(CHAR_BIT == 8 && sizeof(char) == sizeof(unsigned char), "Something is seriously wrong");
		memcpy(preHashbuffer, myKey.c_str(), sizeof(char) * myKey.length());
		memcpy(preHashbuffer + myKey.length(), ServerGlobalGUID, 36);

		unsigned char hash[SHA_DIGEST_LENGTH + 1];
		SHA1(preHashbuffer, preHashSize, hash);
		preHashbuffer[preHashSize] = '\0';
		hash[SHA_DIGEST_LENGTH] = '\0';
		std::cout << "preHash: [" << preHashbuffer << "]" << std::endl;
		std::cout << "SHA1: [";
		for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++)
		{
			unsigned char value = hash[i];
			char majorDigit = value / 16;
			char minorDigit = value % 16;
			if (majorDigit > 9)
			{
				majorDigit += 'A' - 10;
			}
			else
			{
				majorDigit += '0';
			}
			if (minorDigit > 9)
			{
				minorDigit += 'A' - 10;
			}
			else
			{
				minorDigit += '0';
			}
			std::cout << " 0x" << majorDigit << minorDigit;
		}
		std::cout << "]" << std::endl;


		myAcceptKey = Tools::base64_encode(hash, SHA_DIGEST_LENGTH);
	}
}
