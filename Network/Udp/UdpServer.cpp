//===============
// UdpServer.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#pragma comment (lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include "Core/Application.h"
#include "UdpServer.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//==================
// Con-/Destructors
//==================

UdpServer::UdpServer():
uSocket(INVALID_SOCKET)
{}

UdpServer::~UdpServer()
{
Close();
}


//========
// Common
//========

VOID UdpServer::Close()
{
if(uSocket!=INVALID_SOCKET)
	{
	closesocket(uSocket);
	WSACleanup();
	uSocket=INVALID_SOCKET;
	Application::Current->Loop.Remove(this);
	}
}

VOID UdpServer::Listen(WORD uport)
{
if(uSocket!=INVALID_SOCKET)
	Close();
WSADATA wsa;
INT istatus=WSAStartup(MAKEWORD(2,2), &wsa);
if(istatus!=0)
	{
	DebugPrint("UdpServer - WSAStarup failed (%i)\n", istatus);
	return;
	}
addrinfo addr;
ZeroMemory(&addr, sizeof(addr));
addr.ai_family=AF_INET;
addr.ai_socktype=SOCK_DGRAM;
addr.ai_protocol=IPPROTO_UDP;
addr.ai_flags=AI_PASSIVE;
addrinfo* paddr=nullptr;
CHAR pport[8];
StringPrint(pport, 8, "%u", (UINT)uport);
istatus=getaddrinfo(NULL, pport, &addr, &paddr);
if(istatus!=0)
	{
	DebugPrint("UdpServer - getaddrinfo failed (%i)\n", istatus);
	WSACleanup();
	return;
	}
SOCKET sock=socket(paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol);
if(sock==INVALID_SOCKET)
	{
	DebugPrint("UdpServer - socket() failed (%i)\n", WSAGetLastError());
	freeaddrinfo(paddr);
	WSACleanup();
	return;
	}
istatus=bind(sock, paddr->ai_addr, (INT)paddr->ai_addrlen);
if(istatus==SOCKET_ERROR)
	{
	DebugPrint("UdpServer - bind() failed (%i)\n", WSAGetLastError());
	closesocket(sock);
	freeaddrinfo(paddr);
	WSACleanup();
	return;
	}
freeaddrinfo(paddr);
uSocket=sock;
Application::Current->Loop.Add(this, &UdpServer::OnLoop);
}


//================
// Common Private
//================

VOID UdpServer::OnLoop()
{
CHAR pbuf[512];
INT ilen=recv(uSocket, pbuf, 512, 0);
if(ilen==0||ilen==SOCKET_ERROR)
	return;
WORD usize=(WORD)ilen;
Handle<UdpMessage> hmsg=new UdpMessage(pbuf, usize);
MessageReceived(this, hmsg);
}

}}