//===============
// MemoryClass.h
//===============

#pragma once


//===========
// Namespace
//===========

namespace Devices {


//========
// Memory
//========

class Memory: public Object
{
public:
	// Con-/Destructors
	Memory();

	// Common
	Handle<Int64> Available;
	Handle<Int64> Total;
	VOID Update();
};

}
