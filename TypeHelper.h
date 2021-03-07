//==============
// TypeHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "MemoryHelper.h"


//==================
// Integrated Types
//==================

typedef double DOUBLE;


//========
// Limits
//========

#define MAX_UINT 0xFFFFFFFF

#ifdef BITS32
#define MAX_SIZE_T 0xFFFFFFFF
#endif

#ifdef BITS64
#define MAX_SIZE_T 0xFFFFFFFFFFFFFFFF
#endif
