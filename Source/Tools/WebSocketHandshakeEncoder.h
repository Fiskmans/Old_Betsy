#pragma once
#include <string>
#include <exception>
#include <bitset>


typedef void* PVOID;
typedef PVOID BCRYPT_ALG_HANDLE;

namespace Tools
{
	class WebSocketHandshakeEncoder
	{
	public:
		class Not_Ready : public std::exception
		{
		public:
			Not_Ready(const std::string& aMessage) : std::exception(aMessage.c_str()) {}
		};

		WebSocketHandshakeEncoder();
		~WebSocketHandshakeEncoder();

		bool ValidateResponse(const std::string& aResponseHeader);

		std::string Generate();
		std::string operator()();
		operator std::string();

		WebSocketHandshakeEncoder& SetUserAgent(const std::string& aValue);
		WebSocketHandshakeEncoder& SetHost(const std::string& aHost);
		WebSocketHandshakeEncoder& SetVersion(unsigned short aVersion);

	private:
		void GenerateNonce();
		enum class RequiredFields
		{
			Host,
			SubHost,
			Version,
			Count
		};
		BCRYPT_ALG_HANDLE myAlgHandle;

		std::bitset<static_cast<int>(RequiredFields::Count)> mySetValues;

		std::string myUserAgent;
		std::string myHost;
		std::string mySubHost;
		std::string myKey;
		unsigned short myVersion;

		std::string myAcceptKey;
	};
}

