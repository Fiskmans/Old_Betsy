#include <pch.h>
#include "NetworkHelpers.h"

struct sockaddr;

bool TranslateAddress(const std::string& aAddress, sockaddr* aAddressTarget, bool aAllowFailure, std::function<void(std::string,bool)> aPrinter)
{

	if (inet_pton(AF_INET, aAddress.c_str(), &aAddressTarget) != 1)
	{
		struct addrinfo* ptr = NULL;
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		bool addressFound = false;
		while (!addressFound && !aAllowFailure)
		{
			int error = getaddrinfo(aAddress.c_str(), NULL, &hints, &ptr);
			if (error)
			{
				error = WSAGetLastError();
				switch (error)
				{
				case WSAHOST_NOT_FOUND:
					aPrinter("host not found",true);
					break;
				default:
					aPrinter("could not get address info with error: " + std::to_string(error),true);
					break;
				}
				if (aAllowFailure)
				{
					return false;
				}
			}
			else
			{
				struct addrinfo* result = ptr;

				while (result)
				{
					using namespace std::string_literals;
					char host[NI_MAXHOST];
					if (getnameinfo(result->ai_addr, result->ai_addrlen, host, NI_MAXHOST, NULL, NULL, 0) != NULL)
					{
						aPrinter("wsaerror: " + std::to_string(WSAGetLastError()),true);
						if (aAllowFailure)
						{
							return false;
						}
					}
					else
					{
						//std::cout << "host: " << host << std::endl;
						char address[INET6_ADDRSTRLEN];
						if (inet_ntop(result->ai_family, result->ai_addr, address, INET6_ADDRSTRLEN) != NULL)
						{
							aPrinter("Address: "s + address + std::string(16 - strlen(address), ' ') + host,false);
							memcpy(aAddressTarget, result->ai_addr, result->ai_addrlen);


							addressFound = true;
							break;
						}
						else
						{
							aPrinter("could not translate address",true);
							if (aAllowFailure)
							{
								return false;
							}
						}
					}
					result = result->ai_next;
				}
			}
			freeaddrinfo(ptr);
		}
	}
	aPrinter("Address parsing done",false);
	return true;
}

std::string ReadableAddress(sockaddr* aAddress)
{

	std::string out;

	char addBuf[INET6_ADDRSTRLEN];
	if (inet_ntop(((sockaddr_in*)aAddress)->sin_family, &((sockaddr_in*)aAddress)->sin_addr, addBuf, INET6_ADDRSTRLEN) != NULL)
	{
		out += addBuf;
	}
	else
	{
		out += "[Could not translate address]";
	}

	out += ":" + std::to_string(htons(((sockaddr_in*)aAddress)->sin_port));

	return out;
}
