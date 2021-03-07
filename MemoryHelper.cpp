//==================
// MemoryHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <malloc.h>
#include "MemoryHelper.h"


//============
// Allocation
//============

VOID* AlignedAlloc(SIZE_T size, SIZE_T align)
{
return _aligned_malloc(size, align);
}

VOID AlignedFree(VOID* pbuf)
{
_aligned_free(pbuf);
}

VOID* AlignedRealloc(VOID* pbuf, SIZE_T size, SIZE_T align)
{
return _aligned_realloc(pbuf, size, align);
}

VOID* Alloc(SIZE_T size)
{
return malloc(size);
}

VOID* DeviceAlloc(SIZE_T size)
{
return _aligned_malloc(size, 16);
}

VOID DeviceFree(VOID* pbuf)
{
_aligned_free(pbuf);
}

VOID Free(VOID* pbuf)
{
free(pbuf);
}
