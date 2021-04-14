//========
// File.h
//========

#pragma once


//=======
// Using
//=======

#include "Storage/File.h"
#include "FileHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//======
// File
//======

class File: public Storage::File
{
public:
	// Con-/Destructors
	File(Handle<String> Path, UINT64 Size=0);
	~File();

	// Common
	VOID Append();
	VOID Close();
	Handle<String> GetPath();
	UINT64 GetPosition()const { return uPosition; }

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;
	SIZE_T Read(VOID* Buffer, SIZE_T SIze, BOOL* Cancel);

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Container
	FileSize GetSize()override;
	BOOL Seek(UINT64 Position)override;

	// Storage.File
	BOOL Create(FileCreateMode Create=FileCreateMode::OpenExisting, FileAccessMode Access=FileAccessMode::ReadOnly, FileShareMode Share=FileShareMode::ShareRead)override;
	BOOL SetSize(UINT64 Size)override;

private:
	// Common
	VOID DoClose();
	CriticalSection cCriticalSection;
	HANDLE hFile;
	Handle<String> hPath;
	UINT64 uPosition;
	UINT64 uSize;
};

}}