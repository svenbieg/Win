//=============
// Gateway.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#pragma comment(lib, "Iphlpapi.lib")

#include <winsock2.h>
#include <iphlpapi.h>
#include "Gateway.h"


//===========
// Namespace
//===========

namespace Network {


//=========
// Globals
//=========

Handle<IpAddress> hGatewayIp;


//========
// Common
//========

Handle<IpAddress> Gateway::GetIp()
{
if(hGatewayIp)
	return hGatewayIp;
UINT uflags=GAA_FLAG_INCLUDE_GATEWAYS;
ULONG usize=0;
GetAdaptersAddresses(AF_INET, uflags, nullptr, nullptr, &usize);
if(!usize)
	return nullptr;
PIP_ADAPTER_ADDRESSES pinfo=(PIP_ADAPTER_ADDRESSES)Alloc(usize);
if(!pinfo)
	return nullptr;
GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_GATEWAYS, nullptr, pinfo, &usize);
PIP_ADAPTER_ADDRESSES paddr=pinfo;
while(paddr)
	{
	auto pgateway=paddr->FirstGatewayAddress;
	if(pgateway)
		{
		auto paddr=(sockaddr_in*)pgateway->Address.lpSockaddr;
		hGatewayIp=new IpAddress(paddr->sin_addr.S_un.S_addr);
		break;
		}
	paddr=paddr->Next;
	}
Free(pinfo);
return hGatewayIp;
}

}