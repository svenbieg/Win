//===================
// TcpConnection.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include "Core/Application.h"
#include "TcpConnection.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//==================
// Con-/Destructors
//==================

TcpConnection::TcpConnection(SOCKET sock):
hThis(this),
uSocket(sock),
uStatus(TcpConnectionStatus::Open),
uTimeClose(0),
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
hThis=nullptr;
}

VOID TcpConnection::SetTimeout(UINT utimeout)
{
uTimeout=utimeout;
uTimeClose=GetTickCount64()+uTimeout;
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
UINT uwrite=(UINT)MIN(usize, 2048);
UINT uwritten=send(uSocket, (CHAR const*)pbuf, uwrite, 0);
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
	if(uTimeClose==0)
		return;
	UINT64 utime=GetTickCount64();
	if(utime>uTimeClose)
		Close();
	return;
	}
InputBuffer->Write(pbuf, ilen);
DataReceived(this);
if(uTimeout)
	SetTimeout(uTimeout);
}

}}