#include <pch.h>
#include "CurlHelper.h"
#include <sstream>
#include "curl/curl.h"

typedef std::stringstream BufferType;


namespace Tools
{
	namespace Helpers
	{
		size_t my_write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
		{
			BufferType& buffer = *reinterpret_cast<BufferType*>(userdata);
			if (size != 1) { throw std::invalid_argument("size cannot be anything other than 1 according to https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html"); }
			buffer.write(ptr, nmemb);
#ifdef LOG_NET_STREAMS
#ifdef LOG_LESS
			std::cout << "Data recieved [" << nmemb << "]" << std::endl;
#else
			char* copy = new char[nmemb + 1];
			memcpy(copy, ptr, nmemb);
			copy[nmemb] = '\0';
			std::cout << "Data recieved [" << nmemb << "]" << copy << std::endl;
			delete[] copy;
#endif // LOG_LESS
#endif // LOG_NET_STREAMS

			return nmemb;
		}

		size_t TokenFilter(char* ptr, size_t size, size_t nmemb, void* userdata)
		{
#ifdef LOG_NET_STREAMS
#ifdef LOG_LESS
			std::cout << "Data recieved [" << nmemb << "]" << std::endl;
#else
			char* copy = new char[nmemb + 1];
			memcpy(copy, ptr, nmemb);
			copy[nmemb] = '\0';
			std::cout << "Data recieved [" << nmemb << "]" << copy << std::endl;
			delete[] copy;
#endif
#endif // LOG_NET_STREAMS

			const char* filter = "x-token: ";
			if (memcmp(filter, ptr, strlen(filter)) == 0)
			{
				BufferType& buffer = *reinterpret_cast<BufferType*>(userdata);
				if (size != 1) { throw std::invalid_argument("size cannot be anything other than 1 according to https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html"); }
				buffer.write(ptr + strlen(filter), nmemb - strlen(filter));
#ifdef LOG_NET_STREAMS
				std::cout << "token filter matched" << std::endl;
				std::cout << "token: " << buffer.str() << std::endl;
#endif
			}
			if (size != 1) { throw std::invalid_argument("size cannot be anything other than 1 according to https://curl.haxx.se/libcurl/c/CURLOPT_WRITEFUNCTION.html"); }

			return nmemb;
		}
	}

	std::string SimpleAuthOp(CURL* aCurl, const std::string& aUrl, std::string& aCurrentToken)//data, sizeofdata,url,recivebuffer
	{
		BufferType buffer;
		BufferType tokenBuffer;
		CURLcode res;
	#ifdef LOG_NET_STREAMS
		std::cout << "requesting: " << aUrl << std::endl;
		char errorBuffer[CURL_ERROR_SIZE];
	#endif // LOG_NET_STREAMS
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: application/json");
		headers = curl_slist_append(headers, "charsets: utf-8");
		headers = curl_slist_append(headers, ("X-Token: " + aCurrentToken).c_str());
		headers = curl_slist_append(headers, ("X-Username: " + aCurrentToken).c_str());
		curl_easy_setopt(aCurl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(aCurl, CURLOPT_URL, aUrl.c_str());

		curl_easy_setopt(aCurl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(aCurl, CURLOPT_SSL_VERIFYPEER, 0L);

		curl_easy_setopt(aCurl, CURLOPT_POST, 0L);
		curl_easy_setopt(aCurl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(aCurl, CURLOPT_WRITEFUNCTION, &Helpers::my_write_callback);
		curl_easy_setopt(aCurl, CURLOPT_HEADERDATA, &tokenBuffer);
		curl_easy_setopt(aCurl, CURLOPT_HEADERFUNCTION, &Helpers::TokenFilter);
	#ifdef LOG_NET_STREAMS
		curl_easy_setopt(aCurl, CURLOPT_ERRORBUFFER, errorBuffer);
	#endif // LOG_NET_STREAMS
		res = curl_easy_perform(aCurl);
		if (res == CURLE_OK)
		{
			std::string newToken = tokenBuffer.str();
			if (newToken.length() > 0)
			{
				newToken.erase(newToken.find_last_not_of(" \n\r\t") + 1);
				aCurrentToken = newToken;
	#ifdef LOG_NET_STREAMS
				std::cout << "new token [" << newToken << "]" << std::endl;
	#endif // LOG_NET_STREAMS

			}
			return buffer.str();
		}
	#ifdef LOG_NET_STREAMS
		std::cout << "Got curl error: " << errorBuffer << std::endl;
	#endif // LOG_NET_STREAMS

		return "";
	}

}