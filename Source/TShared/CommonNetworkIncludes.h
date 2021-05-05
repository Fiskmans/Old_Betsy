#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

template<class T>
int PASCAL FAR SimpleSend(
	_In_ SOCKET s,
	_In_ T& aToSend,
	_In_ int flags,
	_In_reads_bytes_opt_(tolen) const struct sockaddr FAR* to,
	_In_ int tolen)
{
	return sendto(s, reinterpret_cast<const char*>(&aToSend), sizeof(T), flags, to, tolen);
}