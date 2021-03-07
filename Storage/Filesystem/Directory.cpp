//===============
// Directory.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Directory.h"
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

Directory::Directory(Handle<String> hname, Handle<String> hpath, Handle<Storage::Directory> hparent):
Storage::Directory(hname, "Storage.Filesystem.Directory", hparent),
hFind(NULL),
hPath(hpath)
{}


//===================
// Storage.Directory
//===================

VOID Directory::Close()
{
FindClose();
}

Handle<Storage::File> Directory::CreateFile(Handle<String> hpath, FileCreateMode create, FileAccessMode access, FileShareMode share)
{
ScopedLock lock(cCriticalSection);
Handle<String> hfp=new String("%s\\%s", hPath->Begin(), hpath->Begin());
Handle<File> hfile=new File(hfp);
if(!hfile->Create(create, access, share))
	return nullptr;
return hfile;
}

Handle<Convertible> Directory::Get(Handle<String> hpath)
{
if(!hpath)
	return nullptr;
ScopedLock lock(cCriticalSection);
auto ppath=hpath->Begin();
UINT upos=0;
while(PathIsSeparator(ppath[upos]))
	upos++;
UINT uclen=PathGetComponentLength(&ppath[upos]);
if(!uclen)
	return this;
Handle<String> hname=new String(uclen, &ppath[upos]);
upos+=uclen;
Handle<String> hitempath=new String("%s\\%s", hPath->Begin(), hname->Begin());
WIN32_FIND_DATA fd;
ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));
HANDLE hfind=FindFirstFileEx(hitempath->Begin(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
if(hfind==INVALID_HANDLE_VALUE)
	hfind=NULL;
if(!hfind)
	return nullptr;
::FindClose(hfind);
if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
	{
	Handle<Directory> hsub=new Directory(hname, hitempath, this);
	if(ppath[upos]==0)
		return hsub;
	return hsub->Get(&ppath[upos]);
	}
return new File(hitempath, FileSize(fd.nFileSizeHigh, fd.nFileSizeLow));
}


//==================
// Common Protected
//==================

VOID Directory::FindClose()
{
if(!hFind)
	return;
cCriticalSection.Unlock();
::FindClose(hFind);
hCurrent=nullptr;
hFind=NULL;
}

Handle<Convertible> Directory::FindCurrent()
{
return hCurrent;
}

Handle<Convertible> Directory::FindFirst()
{
cCriticalSection.Lock();
WIN32_FIND_DATA fd;
ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));
Handle<String> hmask=new String("%s\\*.*", hPath->Begin());
hFind=FindFirstFileEx(hmask->Begin(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
if(hFind==INVALID_HANDLE_VALUE)
	hFind=NULL;
if(!hFind)
	{
	cCriticalSection.Unlock();
	return nullptr;
	}
while(fd.cFileName[0]=='.')
	{
	if(!FindNextFile(hFind, &fd))
		{
		FindClose();
		cCriticalSection.Unlock();
		return nullptr;
		}
	}
Handle<String> hpath=new String("%s\\%s", hPath->Begin(), fd.cFileName);
if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
	{
	hCurrent=new Directory(fd.cFileName, hpath, this);
	}
else
	{
	hCurrent=new File(hpath, FileSize(fd.nFileSizeHigh, fd.nFileSizeLow));
	}
return hCurrent;
}

Handle<Convertible> Directory::FindNext()
{
if(!hFind)
	return nullptr;
WIN32_FIND_DATA fd;
ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));
if(!FindNextFile(hFind, &fd))
	{
	FindClose();
	return nullptr;
	}
Handle<String> hpath=new String("%s\\%s", hPath->Begin(), fd.cFileName);
if(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
	{
	hCurrent=new Directory(fd.cFileName, hpath, this);
	}
else
	{
	hCurrent=new File(hpath, FileSize(fd.nFileSizeHigh, fd.nFileSizeLow));
	}
return hCurrent;
}

}}