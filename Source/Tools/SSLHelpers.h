#pragma once

typedef struct ssl_st SSL;

namespace Tools
{
	namespace SSL
	{
		void log_ssl_error();

		int RecvBytes(::SSL* aSSL, char* aBuffer, size_t aByteCount);
		int SendBytes(::SSL* aSSL, const char* aBuffer, size_t aByteCount);
		int RecvPacket(::SSL* aSSL, std::stringstream& aOut, const char* aEOTFilter = nullptr);
		int SendPacket(::SSL* aSSL, const std::string& buf);
	}
}

