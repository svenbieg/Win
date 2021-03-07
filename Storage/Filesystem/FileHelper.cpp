//================
// FileHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <fileapi.h>
#include "FileHelper.h"
#include "PathHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//========
// Common
//========

BOOL CreateDirectoryTree(Handle<String> hpath)
{
if(!hpath)
	return false;
if(DirectoryExists(hpath))
	return true;
auto ppath=hpath->Begin();
UINT uname=0;
UINT unamelen=PathGetName(ppath, &uname);
if(unamelen>0)
	{
	Handle<String> hparent=new String(uname-1, ppath);
	if(!CreateDirectoryTree(hparent))
		return false;
	}
return CreateDirectory(ppath, nullptr);
}

BOOL DeleteDirectoryTree(Handle<String> hpath)
{
if(!hpath)
	return false;
if(!DirectoryExists(hpath))
	return false;
return RemoveDirectory(hpath->Begin());
}

BOOL DirectoryExists(Handle<String> hpath)
{
if(!hpath)
	return false;
WCHAR ppath[MAX_PATH];
GetEnvironmentVariableW(L"LOCALAPPDATA", ppath, MAX_PATH);
if(StringCompare(ppath, hpath->Begin(), 0, false)==0)
	return true;
StringCopy(ppath, MAX_PATH, hpath->Begin());
CREATEFILE2_EXTENDED_PARAMETERS params;
ZeroMemory(&params, sizeof(CREATEFILE2_EXTENDED_PARAMETERS));
params.dwSize=sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
HANDLE hfile=CreateFile2(ppath, GENERIC_READ, FILE_SHARE_ALL, OPEN_EXISTING, &params);
if(hfile==INVALID_HANDLE_VALUE)
	return false;
if(!hfile)
	return false;
CloseHandle(hfile);
if(params.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
	return true;
return false;
}

BOOL FileDelete(Handle<String> hpath)
{
if(!hpath)
	return false;
if(FileExists(hpath))
	return DeleteFile(hpath->Begin());
if(DirectoryExists(hpath))
	return DeleteDirectoryTree(hpath);
return false;
}

BOOL FileExists(Handle<String> hpath)
{
if(!hpath)
	return false;
WCHAR ppath[MAX_PATH];
StringCopy(ppath, MAX_PATH, hpath->Begin());
CREATEFILE2_EXTENDED_PARAMETERS params;
ZeroMemory(&params, sizeof(CREATEFILE2_EXTENDED_PARAMETERS));
params.dwSize=sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
HANDLE hfile=CreateFile2(ppath, FILE_READ_ATTRIBUTES, FILE_SHARE_ALL, OPEN_EXISTING, &params);
if(hfile==INVALID_HANDLE_VALUE)
	return false;
if(!hfile)
	return false;
CloseHandle(hfile);
return (params.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;
}

UINT FileGetAccessMode(FileAccessMode access)
{
switch(access)
	{
	case FileAccessMode::ReadOnly:
		return GENERIC_READ;
	case FileAccessMode::ReadWrite:
		return GENERIC_READ|GENERIC_WRITE;
	}
return 0;
}

BOOL FileGetBasicInfo(Handle<String> hpath, FILE_BASIC_INFO& cinfo)
{
if(!hpath)
	return false;
WCHAR ppath[MAX_PATH];
StringCopy(ppath, MAX_PATH, hpath->Begin());
HANDLE hfile=CreateFile2(ppath, FILE_READ_ATTRIBUTES, FILE_SHARE_ALL, OPEN_EXISTING, nullptr);
if(hfile==INVALID_HANDLE_VALUE)
	return false;
if(!hfile)
	return false;
BOOL b=GetFileInformationByHandleEx(hfile, FileBasicInfo, &cinfo, sizeof(FILE_BASIC_INFO));
CloseHandle(hfile);
return b;
}

UINT FileGetCreateMode(FileCreateMode create)
{
switch(create)
	{
	case FileCreateMode::CreateAlways:
		return CREATE_ALWAYS;
	case FileCreateMode::CreateNew:
		return CREATE_NEW;
	case FileCreateMode::OpenAlways:
		return OPEN_ALWAYS;
	case FileCreateMode::OpenExisting:
		return OPEN_EXISTING;
	}
return 0;
}

UINT FileGetShareMode(FileShareMode share)
{
switch(share)
	{
	case FileShareMode::Exclusive:
		return 0;
	case FileShareMode::None:
		return 0;
	case FileShareMode::ShareRead:
		return FILE_SHARE_READ;
	case FileShareMode::ShareWrite:
		return FILE_SHARE_READ|FILE_SHARE_WRITE;
	}
return 0;
}

UINT GetFileCount(Handle<String> hmask)
{
if(!hmask)
	return 0;
WIN32_FIND_DATA fd;
ZeroMemory(&fd, sizeof(WIN32_FIND_DATA));
HANDLE hfind=FindFirstFileEx(hmask->Begin(), FindExInfoBasic, &fd, FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
if(!hfind||hfind==INVALID_HANDLE_VALUE)
	return 0;
UINT count=1;
while(FindNextFile(hfind, &fd)!=0)
	count++;
FindClose(hfind);
return count;
}

UINT64 GetFileSize(HANDLE hfile)
{
if(!hfile)
	return 0;
LARGE_INTEGER li;
if(!GetFileSizeEx(hfile, &li))
	return 0;
return li.QuadPart;
}

Handle<String> GetNextFileName(Handle<String> hpath)
{
if(!hpath)
	return nullptr;
auto ppath=hpath->Begin();
UINT upos=0;
UINT unamelen=PathGetName(ppath, &upos);
if(!unamelen)
	return nullptr;
UINT udirlen=PathGetDirectoryLength(ppath);
Handle<String> hname=new String(unamelen, &ppath[upos]);
UINT uext=0;
PathGetExtension(ppath, &uext);
TCHAR ptmp[MAX_PATH];
INT i=0;
if(hname->Scan("%s (%i)", ptmp, MAX_PATH, &i)==2)
	{
	hname=new String("%s (%i)", ptmp, i+1);
	}
else
	{
	hname=new String("%s (1)", hname->Begin());
	}
if(!udirlen)
	return new String("%s.%s", hname->Begin(), &ppath[uext]);
Handle<String> hdir=new String(udirlen, ppath);
return new String("%s%s.%s", hdir->Begin(), hname->Begin(), &ppath[uext]);
}

}}