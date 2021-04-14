//===============
// TcpClient.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#pragma comment (lib, "Ws2_32.lib")

#include <ws2tcpip.h>
#include "Core/Application.h"
#include "TcpClient.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpClient::TcpClient():
hConnectedEvent(INVALID_HANDLE_VALUE),
uPort(0),
uSocket(INVALID_SOCKET)
{}

TcpClient::~TcpClient()
{
CloseInternal();
}


//========
// Common
//========

VOID TcpClient::Close()
{
if(uSocket==INVALID_SOCKET)
	return;
CloseInternal();
Closed(this);
}

VOID TcpClient::Connect(Handle<String> hurl, WORD uport)
{
Close();
WSADATA wsa;
INT istatus=WSAStartup(MAKEWORD(2,2), &wsa);
if(istatus!=0)
	{
	DebugPrint("TcpClient - WSAStarup failed (%i)\n", istatus);
	return;
	}
SOCKET sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if(sock==INVALID_SOCKET)
	{
	DebugPrint("TcpClient - socket() failed (%i)\n", WSAGetLastError());
	WSACleanup();
	return;
	}
INT inoblock=1;
istatus=ioctlsocket(sock, FIONBIO, (ULONG*)&inoblock);
if(istatus==SOCKET_ERROR)
	{
	DebugPrint("TcpServer - ioctlsocket() failed (%i)\n", WSAGetLastError());
	closesocket(sock);
	WSACleanup();
	return;
	}
hUrl=hurl;
uPort=uport;
uSocket=sock;
Application::Current->Loop.Add(this, &TcpClient::DoConnect);
}


//================
// Common Private
//================

VOID TcpClient::CloseInternal()
{
if(uSocket==INVALID_SOCKET)
	return;
closesocket(uSocket);
WSACleanup();
uSocket=INVALID_SOCKET;
if(hConnectedEvent!=INVALID_HANDLE_VALUE)
	{
	WSACloseEvent(hConnectedEvent);
	hConnectedEvent=INVALID_HANDLE_VALUE;
	}
Application::Current->Loop.Remove(this);
hUrl=nullptr;
uPort=0;
}

VOID TcpClient::DoConnect()
{
if(uSocket==INVALID_SOCKET)
	return;
if(hConnectedEvent==INVALID_HANDLE_VALUE)
	{
	#ifdef _UNICODE
	CHAR phost[MAX_PATH];
	StringCopy(phost, MAX_PATH, hUrl->Begin());
	#else
	LPCSTR phost=hUrl->Begin();
	#endif
	sockaddr_in addr;
	if(!ResolveHostName(phost, &addr))
		{
		Close();
		return;
		}
	addr.sin_port=htons(uPort);
	INT istatus=connect(uSocket, (sockaddr*)&addr, sizeof(addr));
	if(istatus!=S_OK)
		{
		INT istatus=WSAGetLastError();
		if(istatus!=WSAEWOULDBLOCK)
			{
			Close();
			return;
			}
		}
	hConnectedEvent=WSACreateEvent();
	WSAEventSelect(uSocket, hConnectedEvent, FD_CONNECT);
	return;
	}
DWORD dwstatus=WSAWaitForMultipleEvents(1, &hConnectedEvent, true, 10, true);
if(dwstatus==S_OK)
	{
	Application::Current->Loop.Remove(this);
	Handle<TcpConnection> hcon=new TcpConnection(uSocket);
	WSACloseEvent(hConnectedEvent);
	hConnectedEvent=INVALID_HANDLE_VALUE;
	hUrl=nullptr;
	uPort=0;
	uSocket=INVALID_SOCKET;
	Connected(this, hcon);
	return;
	}
if(dwstatus!=WSA_WAIT_TIMEOUT&&dwstatus!=WSA_WAIT_IO_COMPLETION)
	{
	Application::Current->Loop.Remove(this);
	Close();
	return;
	}
}

BOOL TcpClient::ResolveHostName(LPCSTR purl, VOID* pres)
{
addrinfo addr;
ZeroMemory(&addr, sizeof(addr));
addr.ai_family=AF_INET;
addr.ai_socktype=SOCK_STREAM;
addrinfo* presult=nullptr;
INT istatus=getaddrinfo(purl, nullptr, &addr, &presult);
if(presult)
	{
	sockaddr_in* paddr=(sockaddr_in*)pres;
	*paddr=*(sockaddr_in*)(presult->ai_addr);
	freeaddrinfo(presult);
	return true;
	}
return false;
}

}}