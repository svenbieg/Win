//==========
// File.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "File.h"
#include "PathHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//==================
// Con-/Destructors
//==================

File::File(Handle<String> hpath, UINT64 usize):
Storage::File(PathGetName(hpath), "Storage.Filesystem.File"),
hFile(NULL),
hPath(hpath),
uPosition(0),
uSize(usize)
{}

File::~File()
{
DoClose();
}


//========
// Common
//========

VOID File::Append()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return;
uPosition=GetSize();
}

VOID File::Close()
{
ScopedLock lock(cCriticalSection);
DoClose();
}

Handle<String> File::GetPath()
{
ScopedLock lock(cCriticalSection);
return hPath;
}


//==============
// Input-Stream
//==============

SIZE_T File::Available()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
uSize=GetFileSize(hFile);
UINT64 available=uSize-uPosition;
if(available>MAX_SIZE_T)
	return MAX_SIZE_T;
return (SIZE_T)available;
}

SIZE_T File::Read(VOID* pbuf, SIZE_T usize)
{
return Read(pbuf, usize, nullptr);
}

SIZE_T File::Read(VOID* pbufv, SIZE_T usize, BOOL* pcancel)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
if(!pbufv)
	{
	uSize=GetFileSize(hFile);
	UINT64 available=uSize-uPosition;
	UINT64 copy=MIN(usize, available);
	uPosition+=copy;
	return (SIZE_T)copy;
	}
auto pbuf=(BYTE*)pbufv;
OVERLAPPED ov;
ZeroMemory(&ov, sizeof(OVERLAPPED));
SIZE_T upos=0;
while(upos<usize)
	{
	UINT ucopy=(UINT)MIN(usize-upos, 1024*1024);
	DWORD uread=0;
	ov.Offset=(UINT)uPosition;
	ov.OffsetHigh=(UINT)(uPosition>>32);
	if(!ReadFile(hFile, &pbuf[upos], ucopy, &uread, &ov))
		return upos;
	upos+=uread;
	uPosition+=uread;
	if(uread<ucopy)
		return upos;
	if(pcancel)
		{
		if(*pcancel)
			return upos;
		}
	}
return upos;
}


//===============
// Output-Stream
//===============

VOID File::Flush()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return;
FlushFileBuffers(hFile);
}

SIZE_T File::Write(VOID const* pbufv, SIZE_T usize)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
auto pbuf=(BYTE const*)pbufv;
OVERLAPPED ov;
ZeroMemory(&ov, sizeof(OVERLAPPED));
SIZE_T upos=0;
while(upos<usize)
	{
	UINT ucopy=(UINT)MIN(usize-upos, 0x10000000);
	DWORD uwritten=0;
	ov.Offset=(UINT)uPosition;
	ov.OffsetHigh=(UINT)(uPosition>>32);
	if(!WriteFile(hFile, &pbuf[upos], ucopy, &uwritten, &ov))
		break;
	upos+=uwritten;
	pbuf+=uwritten;
	uPosition+=uwritten;
	if(uwritten<ucopy)
		break;
	}
uSize=MAX(uSize, uPosition);
return upos;
}


//===========
// Container
//===========

FileSize File::GetSize()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return uSize;
uSize=GetFileSize(hFile);
return uSize;
}

BOOL File::Seek(UINT64 upos)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return false;
LONG lo=(LONG)upos;
LONG hi=(LONG)(upos>>32);
if(SetFilePointer(hFile, lo, &hi, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
	return false;
uPosition=upos;
return true;
}


//==============
// Storage.File
//==============

BOOL File::Create(FileCreateMode create, FileAccessMode access, FileShareMode share)
{
ScopedLock lock(cCriticalSection);
DoClose();
UINT ucreate=FileGetCreateMode(create);
UINT uacc=FileGetAccessMode(access);
UINT ushare=FileGetShareMode(share);
#ifdef _UNICODE
LPCWSTR ppath=hPath->Begin();
#else
WCHAR ppath[MAX_PATH];
StringCopy(ppath, MAX_PATH, hPath->Begin());
#endif
HANDLE hfile=CreateFile2(ppath, uacc, ushare, ucreate, nullptr);
if(hfile==INVALID_HANDLE_VALUE)
	hfile=NULL;
if(hfile==NULL)
	return false;
hFile=hfile;
uSize=0;
return true;
}

BOOL File::SetSize(UINT64 usize)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return false;
if(!Seek(usize))
	return false;
if(!SetEndOfFile(hFile))
	return false;
uSize=usize;
Seek(0);
return true;
}


//================
// Common Private
//================

VOID File::DoClose()
{
if(hFile!=NULL)
	{
	CloseHandle(hFile);
	hFile=NULL;
	}
uPosition=0;
uSize=0;
}

}}