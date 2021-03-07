//===============
// TcpServer.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#pragma comment (lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include "Runtime/Application.h"
#include "TcpServer.h"

using namespace Runtime;


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpServer::TcpServer():
uSocket(INVALID_SOCKET)
{}

TcpServer::~TcpServer()
{
CloseInternal();
Destroyed(this);
}


//========
// Common
//========

VOID TcpServer::Close()
{
if(uSocket==INVALID_SOCKET)
	return;
CloseInternal();
Closed(this);
}

VOID TcpServer::Listen(WORD uport)
{
if(uSocket!=INVALID_SOCKET)
	{
	DebugPrint("TcpServer - already listening\n");
	return;
	}
WSADATA wsa;
INT istatus=WSAStartup(MAKEWORD(2,2), &wsa);
if(istatus!=0)
	{
	DebugPrint("TcpServer - WSAStarup failed (%i)\n", istatus);
	return;
	}
addrinfo addr;
ZeroMemory(&addr, sizeof(addr));
addr.ai_family=AF_INET;
addr.ai_socktype=SOCK_STREAM;
addr.ai_protocol=IPPROTO_TCP;
addr.ai_flags=AI_PASSIVE;
addrinfo* paddr=nullptr;
CHAR pport[8];
StringPrint(pport, 8, "%u", (UINT)uport);
istatus=getaddrinfo(NULL, pport, &addr, &paddr);
if(istatus!=0)
	{
	DebugPrint("TcpServer - getaddrinfo failed (%i)\n", istatus);
	WSACleanup();
	return;
	}
SOCKET sock=socket(paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol);
if(sock==INVALID_SOCKET)
	{
	DebugPrint("TcpServer - socket() failed (%i)\n", WSAGetLastError());
	freeaddrinfo(paddr);
	WSACleanup();
	return;
	}
INT inoblock=1;
istatus=ioctlsocket(sock, FIONBIO, (ULONG*)&inoblock);
if(istatus==SOCKET_ERROR)
	{
	DebugPrint("TcpServer - ioctlsocket() failed (%i)\n", WSAGetLastError());
	closesocket(sock);
	freeaddrinfo(paddr);
	WSACleanup();
	return;
	}
istatus=bind(sock, paddr->ai_addr, (INT)paddr->ai_addrlen);
if(istatus==SOCKET_ERROR)
	{
	DebugPrint("TcpServer - bind() failed (%i)\n", WSAGetLastError());
	closesocket(sock);
	freeaddrinfo(paddr);
	WSACleanup();
	return;
	}
freeaddrinfo(paddr);
istatus=listen(sock, SOMAXCONN);
if(istatus==SOCKET_ERROR)
	{
	DebugPrint("TcpServer - listen() failed (%i)\n", WSAGetLastError());
	closesocket(sock);
	WSACleanup();
	return;
	}
Application::Current->Loop.Add(this, &TcpServer::OnLoop);
uSocket=sock;
}


//================
// Common Private
//================

VOID TcpServer::CloseInternal()
{
if(uSocket==INVALID_SOCKET)
	return;
closesocket(uSocket);
WSACleanup();
uSocket=INVALID_SOCKET;
Application::Current->Loop.Remove(this);
}

VOID TcpServer::OnLoop()
{
SOCKET sock=accept(uSocket, nullptr, nullptr);
if(sock==INVALID_SOCKET)
	{
	INT ierr=WSAGetLastError();
	if(ierr==WSAEWOULDBLOCK)
		return;
	DebugPrint("TcpServer- accept() failed (%i)\n", ierr);
	return;
	}
Handle<TcpConnection> hcon=new TcpConnection(sock);
ConnectionReceived(this, hcon);
}

}}