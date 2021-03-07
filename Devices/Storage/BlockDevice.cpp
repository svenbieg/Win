//=================
// BlockDevice.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Storage/BlockDevice.h"
#include "FileDevice.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Storage {


Handle<BlockDevice> BlockDevice::GetDefault()
{
return new FileDevice();
}

}}