//==============
// FileDevice.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Storage/BlockDevice.h"
#include "Storage/Filesystem/File.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Storage {


//=============
// File-Device
//=============

class FileDevice: public BlockDevice
{
private:
	// Using
	using File=::Storage::Filesystem::File;
	using FileSize=::Storage::FileSize;

public:
	// Con-/Destructors
	FileDevice();

	// Common
	VOID Erase(UINT64 Offset, UINT Size)override;
	VOID Flush()override;
	UINT GetBlockSize()override;
	UINT64 GetSize()override;
	SIZE_T Read(UINT64 Offset, VOID* Buffer, SIZE_T Size)override;
	BOOL SetSize(UINT64 Size)override;
	SIZE_T Write(UINT64 Offset, VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	UINT64 AvailableOnDisk();
	CriticalSection cCriticalSection;
	Handle<File> hFile;
};

}}