//===========
// Gateway.h
//===========

#pragma once


//=======
// Using
//=======

#include "Network/IpAddress.h"


//===========
// Namespace
//===========

namespace Network {


//=========
// Gateway
//=========

class Gateway
{
public:
	// Common
	static Handle<IpAddress> GetIp();
};

}