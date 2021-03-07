//=============
// UdpServer.h
//=============

#pragma once


//=======
// Using
//=======

#include <WinSock2.h>
#include "Network/Udp/UdpMessage.h"


//===========
// Namespace
//===========

namespace Network {
	namespace Udp {


//============
// Udp-Server
//============

class UdpServer: public Object
{
public:
	// Con-/Destructors
	UdpServer();
	~UdpServer();

	// Common
	VOID Close();
	Event<UdpServer, Handle<UdpMessage>> MessageReceived;
	VOID Listen(WORD Port);

private:
	// Common
	VOID OnLoop();
	SOCKET uSocket;
};

}}