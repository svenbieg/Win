//==============
// TextFile.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "FileHelper.h"
#include "PathHelper.h"
#include "TextFile.h"

using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//=========
// Unicode
//=========

const WCHAR wcUnicode=0xFEFF;


//==================
// Con-/Destructors
//==================

TextFile::TextFile(Handle<String> hpath, UINT64 usize):
Storage::File(PathGetName(hpath), "Storage.Filesystem.TextFile"),
hFile(NULL),
hPath(hpath),
uFormat(TextFileFormat::Ansi),
uPosition(0),
uSize(usize)
{}

TextFile::~TextFile()
{
if(hFile!=NULL)
	CloseHandle(hFile);
}


//========
// Common
//========

VOID TextFile::Append()
{
ScopedLock lock(cCriticalSection);
uPosition=uSize;
}

SIZE_T TextFile::Append(Handle<String> htext)
{
ScopedLock lock(cCriticalSection);
if(!htext)
	return 0;
uPosition=uSize;
StreamWriter writer(this, uFormat==TextFileFormat::Unicode);
return writer.Print(htext);
}

VOID TextFile::Clear()
{
ScopedLock lock(cCriticalSection);
ClearInternal();
}

VOID TextFile::Close()
{
ScopedLock lock(cCriticalSection);
if(hFile)
	CloseHandle(hFile);
hFile=NULL;
}

TextFileFormat TextFile::GetFormat()
{
ScopedLock lock(cCriticalSection);
return uFormat;
}

Handle<String> TextFile::GetPath()
{
ScopedLock lock(cCriticalSection);
return hPath;
}

Handle<String> TextFile::Read()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return nullptr;
UINT ulen=(UINT)(uSize-uPosition);
if(uFormat==TextFileFormat::Unicode)
	ulen/=2;
Handle<String> htext=new String(ulen+1, nullptr);
auto pstr=(LPTSTR)htext->Begin();
StreamReader reader(this, uFormat==TextFileFormat::Unicode);
reader.ReadString(pstr, ulen+1);
pstr[ulen]=0;
return htext;
}

VOID TextFile::SetFormat(TextFileFormat format)
{
ScopedLock lock(cCriticalSection);
uFormat=format;
ClearInternal();
}

SIZE_T TextFile::Write(Handle<String> htext)
{
ScopedLock lock(cCriticalSection);
ClearInternal();
StreamWriter writer(this, uFormat==TextFileFormat::Unicode);
return writer.Print(htext);
}


//==============
// Input-Stream
//==============

SIZE_T TextFile::Available()
{
ScopedLock lock(cCriticalSection);
UINT64 available=uSize-uPosition;
if(available>MAX_SIZE_T)
	return MAX_SIZE_T;
return (SIZE_T)available;
}

SIZE_T TextFile::Read(VOID* pbuf, SIZE_T usize)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
OVERLAPPED ov;
ZeroMemory(&ov, sizeof(OVERLAPPED));
SIZE_T upos=0;
while(upos<usize)
	{
	UINT ucopy=(UINT)MIN(usize-upos, 0x10000000);
	DWORD uread=0;
	ov.Offset=(UINT)uPosition;
	ov.OffsetHigh=(UINT)(uPosition>>32);
	if(!ReadFile(hFile, pbuf, ucopy, &uread, &ov))
		return upos;
	upos+=uread;
	uPosition+=uread;
	if(uread<ucopy)
		return upos;
	}
return upos;
}


//===============
// Output-Stream
//===============

VOID TextFile::Flush()
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return;
FlushFileBuffers(hFile);
}

SIZE_T TextFile::Write(VOID const* pbufv, SIZE_T usize)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
BYTE const* pbuf=(BYTE const*)pbufv;
OVERLAPPED ov;
ZeroMemory(&ov, sizeof(OVERLAPPED));
SIZE_T upos=0;
while(upos<usize)
	{
	UINT ucopy=(UINT)MIN(usize-upos, 0x10000000);
	DWORD uwritten=0;
	ov.Offset=(UINT)uPosition;
	ov.OffsetHigh=(UINT)(uPosition>>32);
	if(!WriteFile(hFile, pbuf, ucopy, &uwritten, &ov))
		break;
	upos+=uwritten;
	pbuf+=uwritten;
	uPosition+=uwritten;
	if(uwritten<ucopy)
		break;
	}
if(uPosition>uSize)
	uSize=uPosition;
return upos;
}


//===========
// Container
//===========

FileSize TextFile::GetSize()
{
ScopedLock lock(cCriticalSection);
return uSize;
}

BOOL TextFile::Seek(UINT64 pos)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return false;
LONG lo=(LONG)pos;
LONG hi=(LONG)(pos>>32);
if(SetFilePointer(hFile, lo, &hi, FILE_BEGIN)==INVALID_SET_FILE_POINTER)
	return false;
uPosition=pos;
return true;
}


//==============
// Storage.File
//==============

BOOL TextFile::Create(FileCreateMode create, FileAccessMode access, FileShareMode share)
{
ScopedLock lock(cCriticalSection);
if(hFile)
	return true;
if(!hPath)
	return false;
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
UINT64 size=GetFileSize(hfile);
switch(create)
	{
	case FileCreateMode::CreateAlways:
	case FileCreateMode::CreateNew:
		{
		SetEndOfFile(hfile);
		size=0;
		break;
		}
	case FileCreateMode::OpenAlways:
	case FileCreateMode::OpenExisting:
		{
		if(size<sizeof(WCHAR))
			break;
		WCHAR wc=0;
		if(ReadFile(hfile, &wc, sizeof(WCHAR), nullptr, nullptr))
			{
			if(wc==wcUnicode)
				{
				uFormat=TextFileFormat::Unicode;
				}
			else
				{
				SetFilePointer(hfile, 0, nullptr, FILE_BEGIN);
				}
			}
		break;
		}
	}
hFile=hfile;
uSize=size;
return true;
}

BOOL TextFile::SetSize(UINT64 size)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return false;
if(!Seek(size))
	return false;
if(SetEndOfFile(hFile))
	{
	uSize=size;
	return true;
	}
return false;
}


//================
// Common Private
//================

VOID TextFile::ClearInternal()
{
if(!hFile)
	return;
SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
SetEndOfFile(hFile);
uPosition=0;
uSize=0;
if(uFormat==TextFileFormat::Unicode)
	{
	WriteFile(hFile, &wcUnicode, sizeof(WCHAR), nullptr, nullptr);
	SetEndOfFile(hFile);
	uSize=sizeof(WCHAR);
	uPosition=uSize;
	}
}

}}