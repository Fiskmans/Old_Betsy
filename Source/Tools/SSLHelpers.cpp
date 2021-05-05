#include <pch.h>
#include "SSLHelpers.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "Logger.h"
#include <sstream>

namespace Tools
{
	namespace SSL
	{
		void log_ssl_error()
		{
			int err;
			while (err = ERR_get_error()) {
				char* str = ERR_error_string(err, 0);
				if (!str)
					return;
				SYSNETWORK(str);
			}
		}

		int RecvBytes(::SSL* aSSL, char* aBuffer, size_t aByteCount)
		{
			size_t at = 0;
			size_t left = aByteCount;
			while (left > 0)
			{
				int read = SSL_read(aSSL, aBuffer + at, left);
				if (read < 0) {
					int err = SSL_get_error(aSSL, read);
					if (err == SSL_ERROR_WANT_READ)
						continue;
					if (err == SSL_ERROR_WANT_WRITE)
						continue;
					if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
						return -1;
				}
				left -= read;
				at += read;
				//std::cout << "read " << read << " left " << left << std::endl;
			}
			return 0;
		}

		int SendBytes(::SSL* aSSL, const char* aBuffer, size_t aByteCount)
		{
			//std::cout << "Sending (" << aByteCount << ") bytes of data" << std::endl;
			int wrote = SSL_write(aSSL, aBuffer, aByteCount);
			if (wrote < 0) {
				int err = SSL_get_error(aSSL, wrote);
				switch (err) {
				case SSL_ERROR_WANT_WRITE:
					return 0;
				case SSL_ERROR_WANT_READ:
					return 0;
				case SSL_ERROR_ZERO_RETURN:
				case SSL_ERROR_SYSCALL:
				case SSL_ERROR_SSL:
				default:
					return -1;
				}
			}
			return 0;
		}

		int RecvPacket(::SSL* aSSL, std::stringstream& aOut, const char* aEOTFilter)
		{
			const int bufferSize = 256;
			int len = 0;
			char buf[bufferSize + 1];
			//if (aEOTFilter)
			//{
			//	std::cout << "EOTFilter: [" << aEOTFilter << "]" << std::endl;
			//}
			do {
				len = SSL_read(aSSL, buf, bufferSize);
				if (len > 0)
				{
					aOut.write(buf, len);
					buf[len] = '\0';
					//std::cout << "recieved: (" << len << ")[" << buf << "]" << std::endl;
				}
			} while (len == bufferSize && (!aEOTFilter || aOut.str().find(aEOTFilter) != std::string::npos));
			if (len < 0) {
				int err = SSL_get_error(aSSL, len);
				if (err == SSL_ERROR_WANT_READ)
					return 0;
				if (err == SSL_ERROR_WANT_WRITE)
					return 0;
				if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
					return -1;
			}

			return 0;
		}

		int SendPacket(::SSL* aSSL, const std::string& buf)
		{
			//std::cout << "Sending: (" << buf.length() << ")[" << buf << "]" << std::endl;
			int len = SSL_write(aSSL, buf.c_str(), buf.length());
			if (len < 0) {
				int err = SSL_get_error(aSSL, len);
				switch (err) {
				case SSL_ERROR_WANT_WRITE:
					return 0;
				case SSL_ERROR_WANT_READ:
					return 0;
				case SSL_ERROR_ZERO_RETURN:
				case SSL_ERROR_SYSCALL:
				case SSL_ERROR_SSL:
				default:
					return -1;
				}
			}

			return 0;
		}
	}
}
