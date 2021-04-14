//================
// FileDevice.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Runtime/Application.h"
#include "Storage/Filesystem/FileHelper.h"
#include "Storage/AppDataHelper.h"
#include "FileDevice.h"

using namespace Runtime;
using namespace Storage;
using namespace Storage::Filesystem;


//===========
// Namespace
//===========

namespace Devices {
	namespace Storage {


//==================
// Con-/Destructors
//==================

FileDevice::FileDevice()
{
auto app=Application::Current;
if(!app)
	return;
auto name=app->Name;
if(!name)
	return;
auto local=GetAppDataLocalPath();
if(!local)
	return;
if(!DirectoryExists(local))
	CreateDirectoryTree(local);
auto path=new String("%s\\%s.db", local->Begin(), name->Begin());
hFile=new File(path);
if(!hFile->Create(FileCreateMode::OpenAlways, FileAccessMode::ReadWrite))
	hFile=nullptr;
}


//========
// Common
//========

VOID FileDevice::Erase(UINT64 offset, UINT size)
{
ScopedLock lock(cCriticalSection);
ASSERT(offset%PAGE_SIZE==0);
ASSERT(size%PAGE_SIZE==0);
if(!hFile)
	return;
UINT block_count=size/PAGE_SIZE;
BYTE pbuf[PAGE_SIZE];
FillMemory(pbuf, PAGE_SIZE, -1);
hFile->Seek(offset);
for(UINT block=0; block<block_count; block++)
	hFile->Write(pbuf, PAGE_SIZE);
hFile->Flush();
}

VOID FileDevice::Flush()
{
if(!hFile)
	return;
hFile->Flush();
}

UINT FileDevice::GetBlockSize()
{
if(!hFile)
	return 0;
return PAGE_SIZE;
}

UINT64 FileDevice::GetSize()
{
if(!hFile)
	return 0;
UINT64 size=hFile->GetSize();
UINT64 free=AvailableOnDisk();
return size+free;
}

SIZE_T FileDevice::Read(UINT64 offset, VOID* pbuf, SIZE_T size)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
if(!hFile->Seek(offset))
	return 0;
return hFile->Read(pbuf, size);
}

BOOL FileDevice::SetSize(UINT64 size)
{
if(!hFile)
	return false;
return hFile->SetSize(size);
}

SIZE_T FileDevice::Write(UINT64 offset, VOID const* pbuf, SIZE_T size)
{
ScopedLock lock(cCriticalSection);
if(!hFile)
	return 0;
if(!hFile->Seek(offset))
	return 0;
return hFile->Write(pbuf, size);
}


//================
// Common Private
//================

UINT64 FileDevice::AvailableOnDisk()
{
auto hpath=hFile->GetPath();
TCHAR ppath[4];
StringCopy(ppath, 4, hpath->Begin());
DWORD block_size=0;
DWORD sec_size=0;
DWORD blocks_free=0;
DWORD blocks_total=0;
if(!GetDiskFreeSpace(ppath, &block_size, &sec_size, &blocks_free, &blocks_total))
	return 0;
return (UINT64)blocks_free*block_size*sec_size;
}

}}