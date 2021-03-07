//=================
// MemoryClass.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "MemoryClass.h"


//===========
// Namespace
//===========

namespace Devices {


//==================
// Con-/Destructors
//==================

Memory::Memory()
{
MEMORYSTATUSEX status;
status.dwLength=sizeof(MEMORYSTATUSEX);
GlobalMemoryStatusEx(&status);
Available=new Int64("MemoryAvailable", status.ullAvailPhys);
Total=new Int64("MemoryTotal", status.ullTotalPhys);
}


//========
// Common
//========

VOID Memory::Update()
{
MEMORYSTATUSEX status;
status.dwLength=sizeof(MEMORYSTATUSEX);
GlobalMemoryStatusEx(&status);
Available=status.ullAvailPhys;
}

}
