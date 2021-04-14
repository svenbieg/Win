//=================
// TcpConnection.h
//=================

#pragma once


//=======
// Using
//=======

#include <WinSock2.h>
#include "Storage/Streams/RandomAccessStream.h"
#include "Storage/PacketBuffer.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//========
// Status
//========

enum class TcpConnectionStatus
{
Closed,
Open
};


//================
// Tcp-Connection
//================

class TcpConnection: public Storage::Streams::RandomAccessStream
{
private:
	// Using
	using PacketBuffer=Storage::PacketBuffer;

public:
	// Con-/Destructors
	TcpConnection(SOCKET Socket);
	~TcpConnection();

	// Common
	VOID Close();
	Event<TcpConnection> Closed;
	Event<TcpConnection> DataReceived;
	Event<TcpConnection> DataSent;
	Handle<PacketBuffer> InputBuffer;
	Event<TcpConnection> Send;
	VOID SetTimeout(UINT Timeout);
	VOID Skip() {}

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	VOID CloseInternal();
	VOID OnLoop();
	Handle<TcpConnection> hThis;
	SOCKET uSocket;
	TcpConnectionStatus uStatus;
	UINT64 uTimeClose;
	UINT uTimeout;
};

}}