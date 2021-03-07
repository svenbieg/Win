//============
// TextFile.h
//============

#pragma once


//=======
// Using
//=======

#include "Storage/File.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Filesystem {


//========
// Format
//========

enum class TextFileFormat
{
Ansi,
Unicode
};


//===========
// Text-File
//===========

class TextFile: public Storage::File
{
public:
	// Con-/Destructors
	TextFile(Handle<String> Path, UINT64 Size=0);
	~TextFile();

	// Common
	VOID Append();
	SIZE_T Append(Handle<String> Text);
	VOID Clear();
	VOID Close();
	TextFileFormat GetFormat();
	Handle<String> GetPath();
	BOOL IsUnicode()const { return uFormat==TextFileFormat::Unicode; }
	Handle<String> Read();
	VOID SetFormat(TextFileFormat Format);
	SIZE_T Write(Handle<String> Text);

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T SIze)override;

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
	VOID ClearInternal();
	CriticalSection cCriticalSection;
	HANDLE hFile;
	Handle<String> hPath;
	TextFileFormat uFormat;
	UINT64 uPosition;
	UINT64 uSize;
};

}}