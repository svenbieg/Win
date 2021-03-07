//=============
// TcpClient.h
//=============

#pragma once


//=======
// Using
//=======

#include "TcpConnection.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Tcp {


//============
// Tcp-Client
//============

class TcpClient: public Object
{
public:
	// Con-/Destructors
	TcpClient();
	~TcpClient();
	Event<VOID, TcpClient*> Destroyed;

	// Common
	VOID Close();
	Event<TcpClient> Closed;
	VOID Connect(Handle<String> Address, WORD Port);
	Event<TcpClient, Handle<TcpConnection>> Connected;

private:
	// Common
	VOID CloseInternal();
	VOID DoConnect();
	BOOL ResolveHostName(LPCSTR Url, VOID* Address);
	HANDLE hConnectedEvent;
	Handle<String> hUrl;
	WORD uPort;
	SOCKET uSocket;
};

}}