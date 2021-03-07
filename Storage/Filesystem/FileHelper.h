//==============
// FileHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Common/Storage/FileHelper.h"


//=============
// Definitions
//=============

#define FILE_SHARE_ALL (FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE)


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//========
// Common
//========

BOOL CreateDirectoryTree(Handle<String> Path);
BOOL DeleteDirectoryTree(Handle<String> Path);
BOOL DirectoryExists(Handle<String> Path);
BOOL FileDelete(Handle<String> Path);
BOOL FileExists(Handle<String> Path);
UINT FileGetAccessMode(FileAccessMode Access);
BOOL FileGetBasicInfo(Handle<String> Path, FILE_BASIC_INFO& Info);
UINT FileGetCreateMode(FileCreateMode Create);
UINT FileGetShareMode(FileShareMode Share);
UINT GetFileCount(Handle<String> Mask);
UINT64 GetFileSize(HANDLE File);
Handle<String> GetNextFileName(Handle<String> Path);
inline BOOL IsAccessReadOnly(UINT Access) { return !(Access&(GENERIC_WRITE|FILE_WRITE_DATA|FILE_APPEND_DATA)); }

}}