//===================
// TcpConnection.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include "Runtime/Application.h"
#include "TcpConnection.h"

using namespace Runtime;


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpConnection::TcpConnection(SOCKET sock):
uSocket(sock),
uStatus(TcpConnectionStatus::Open),
uTimeout(0)
{
InputBuffer=new PacketBuffer();
Application::Current->Loop.Add(this, &TcpConnection::OnLoop);
}

TcpConnection::~TcpConnection()
{
CloseInternal();
}


//========
// Common
//========

VOID TcpConnection::Close()
{
if(uStatus==TcpConnectionStatus::Closed)
	return;
CloseInternal();
uStatus=TcpConnectionStatus::Closed;
Closed(this);
}

VOID TcpConnection::SetTimeout(UINT utimeout)
{
UINT64 utime=GetTickCount64();
uTimeout=utime+utimeout;
}


//==============
// Input-Stream
//==============

SIZE_T TcpConnection::Available()
{
return InputBuffer->Available();
}

SIZE_T TcpConnection::Read(VOID* pbuf, SIZE_T usize)
{
return InputBuffer->Read(pbuf, usize);
}


//===============
// Output-Stream
//===============

VOID TcpConnection::Flush()
{
shutdown(uSocket, SD_SEND);
DataSent(this);
}

SIZE_T TcpConnection::Write(VOID const* pbuf, SIZE_T usize)
{
UINT uwrite=MIN(usize, 2048);
UINT uwritten=send(uSocket, (CHAR const*)pbuf, uwrite, 0);
SetTimeout(10000);
if(uwritten>uwrite)
	return 0;
return uwritten;
}


//================
// Common Private
//================

VOID TcpConnection::CloseInternal()
{
if(uSocket!=INVALID_SOCKET)
	{
	shutdown(uSocket, SD_BOTH);
	closesocket(uSocket);
	uSocket=INVALID_SOCKET;
	}
Application::Current->Loop.Remove(this);
InputBuffer->Clear();
}

VOID TcpConnection::OnLoop()
{
Send(this);
CHAR pbuf[512];
INT ilen=recv(uSocket, pbuf, 512, 0);
if(ilen==0||ilen==SOCKET_ERROR)
	{
	if(uTimeout==0)
		return;
	UINT64 utime=GetTickCount64();
	if(utime>uTimeout)
		Close();
	return;
	}
InputBuffer->Write(pbuf, ilen);
DataReceived(this);
}

}}