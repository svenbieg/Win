//===============
// UdpClient.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#pragma comment (lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include "UdpClient.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//========
// Common
//========

VOID UdpClient::Broadcast(WORD uport, VOID const* pmsg, WORD usize)
{
WSADATA wsa;
INT istatus=WSAStartup(MAKEWORD(2,2), &wsa);
if(istatus!=0)
	{
	DebugPrint("UdpClient::Broadcast - WSAStarup failed (%i)\n", istatus);
	return;
	}
SOCKET sock=socket(AF_INET, SOCK_DGRAM, 0);
if(sock==INVALID_SOCKET)
	{
	DebugPrint("UdpClient::Broadcast - socket() failed (%i)\n", WSAGetLastError());
	WSACleanup();
	return;
	}
CHAR broadcast='1';
if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(CHAR))<0)
	{
	DebugPrint("UdpClient::Broadcast - setsockopt() failed\n");
	closesocket(sock);
	WSACleanup();
	return;
	}
sockaddr_in addr;
addr.sin_family=AF_INET;
addr.sin_port=htons(uport);
addr.sin_addr.S_un.S_addr=0xFFFFFFFF;
sendto(sock, (CHAR const*)pmsg, usize, 0, (sockaddr*)&addr, sizeof(sockaddr_in));
closesocket(sock);
WSACleanup();
}

}}