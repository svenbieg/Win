//================
// MemoryHelper.h
//================

#pragma once


//==============
// Architecture
//==============

#if(defined(_M_AMD64)||defined(_M_ARM64))
#define BITS64
#define IF32(x)
#define IF64(x) x
#else
#define BITS32
#define IF32(x) x
#define IF64(x)
#endif


//============
// Allocation
//============

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

VOID* AlignedAlloc(SIZE_T Size, SIZE_T Align);
VOID AlignedFree(VOID* Buffer);
VOID* AlignedRealloc(VOID* Buffer, SIZE_T Size, SIZE_T Align);
VOID* Alloc(SIZE_T Size);
VOID* DeviceAlloc(SIZE_T Size);
VOID DeviceFree(VOID* Buffer);
VOID Free(VOID* Buffer);
