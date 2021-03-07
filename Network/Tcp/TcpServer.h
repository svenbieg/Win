//=============
// TcpServer.h
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
// Tcp-Server
//============

class TcpServer: public Object
{
public:
	// Con-/Destructors
	TcpServer();
	~TcpServer();
	Event<VOID, TcpServer*> Destroyed;

	// Common
	VOID Close();
	Event<TcpServer> Closed;
	Event<TcpServer, Handle<TcpConnection>> ConnectionReceived;
	VOID Listen(WORD Port);

private:
	// Common
	VOID CloseInternal();
	VOID OnLoop();
	SOCKET uSocket;
};

}}
