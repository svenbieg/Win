//====================
// ResourceHelper.cpp
//====================

#include "pch.h"


//=======
// Using
//=======

#include "ResourceHelper.h"


//=======
// Types
//=======

#define RC_TYPE_STRING 6
#define RC_TYPE_ICON 3
#define RC_TYPE_ICONGROUP 14


//============
// Entrypoint
//============

BYTE* pBase=nullptr;


//=========
// Unicode
//=========

#ifdef _UNICODE
typedef IMAGE_RESOURCE_DIR_STRING_U IMAGE_RESOURCE_DIR_STRING_T;
#else
typedef IMAGE_RESOURCE_DIRECTORY_STRING IMAGE_RESOURCE_DIR_STRING_T;
#endif


//======================
// Forward-Declarations
//======================

IMAGE_RESOURCE_DATA_ENTRY* GetResourceData(IMAGE_RESOURCE_DIRECTORY* Parent, UINT Id);
IMAGE_RESOURCE_DATA_ENTRY* GetResourceData(IMAGE_RESOURCE_DIRECTORY* Parent, LPCTSTR Name);
IMAGE_RESOURCE_DIRECTORY* GetResourceDirectory(IMAGE_RESOURCE_DIRECTORY* Parent, UINT Id);
IMAGE_RESOURCE_DIRECTORY* GetResourceDirectory(IMAGE_RESOURCE_DIRECTORY* Parent, LPCTSTR Name);
IMAGE_RESOURCE_DIRECTORY* GetResourceRoot();


//============
// Ressources
//============

IMAGE_RESOURCE_DIRECTORY* pRoot=nullptr;

IMAGE_RESOURCE_DATA_ENTRY* GetResourceData(IMAGE_RESOURCE_DIRECTORY* pparent, UINT uid)
{
BYTE* pdir=(BYTE*)pparent;
UINT upos=sizeof(IMAGE_RESOURCE_DIRECTORY);
upos+=pparent->NumberOfNamedEntries*sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
BYTE* proot=(BYTE*)pRoot;
IMAGE_RESOURCE_DIRECTORY_ENTRY* pentry=(IMAGE_RESOURCE_DIRECTORY_ENTRY*)&pdir[upos];
for(UINT u=1; u<pparent->NumberOfIdEntries; u++)
	{
	if(pentry->Name==uid)
		break;
	upos+=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	pentry=(IMAGE_RESOURCE_DIRECTORY_ENTRY*)&pdir[upos];
	}
if(pentry->DataIsDirectory)
	throw(EINVAL);
return (IMAGE_RESOURCE_DATA_ENTRY*)&proot[pentry->OffsetToData];
}

IMAGE_RESOURCE_DATA_ENTRY* GetResourceData(IMAGE_RESOURCE_DIRECTORY* pparent, LPCTSTR pname)
{
UINT unamelen=StringLength(pname);
ASSERT(unamelen>0);
BYTE* pdir=(BYTE*)pparent;
UINT upos=sizeof(IMAGE_RESOURCE_DIRECTORY);
for(UINT u=0; u<pparent->NumberOfNamedEntries; u++)
	{
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pentry=(IMAGE_RESOURCE_DIRECTORY_ENTRY*)&pdir[upos];
	IMAGE_RESOURCE_DIR_STRING_T* pstr=(IMAGE_RESOURCE_DIR_STRING_T*)&pdir[pentry->NameOffset];
	if(pstr->Length==unamelen&&StringCompare(pstr->NameString, pname, unamelen)==0)
		{
		if(pentry->DataIsDirectory)
			throw(EINVAL);
		BYTE* proot=(BYTE*)pRoot;
		return (IMAGE_RESOURCE_DATA_ENTRY*)&proot[pentry->OffsetToData];
		}
	upos+=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	}
throw(EINVAL);
return 0;
}

IMAGE_RESOURCE_DIRECTORY* GetResourceDirectory(IMAGE_RESOURCE_DIRECTORY* pparent, UINT uid)
{
BYTE* pdir=(BYTE*)pparent;
UINT upos=sizeof(IMAGE_RESOURCE_DIRECTORY);
upos+=pparent->NumberOfNamedEntries*sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
for(UINT u=0; u<pparent->NumberOfIdEntries; u++)
	{
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pentry=(IMAGE_RESOURCE_DIRECTORY_ENTRY*)&pdir[upos];
	if(pentry->Name==uid)
		{
		if(!pentry->DataIsDirectory)
			throw(EINVAL);
		BYTE* proot=(BYTE*)pRoot;
		return (IMAGE_RESOURCE_DIRECTORY*)&proot[pentry->OffsetToDirectory];
		}
	upos+=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	}
throw(EINVAL);
return 0;
}

IMAGE_RESOURCE_DIRECTORY* GetResourceDirectory(IMAGE_RESOURCE_DIRECTORY* pparent, LPCTSTR pname)
{
UINT unamelen=StringLength(pname);
ASSERT(unamelen>0);
BYTE* pdir=(BYTE*)pparent;
UINT upos=sizeof(IMAGE_RESOURCE_DIRECTORY);
for(UINT u=0; u<pparent->NumberOfNamedEntries; u++)
	{
	IMAGE_RESOURCE_DIRECTORY_ENTRY* pentry=(IMAGE_RESOURCE_DIRECTORY_ENTRY*)&pdir[upos];
	IMAGE_RESOURCE_DIR_STRING_T* pstr=(IMAGE_RESOURCE_DIR_STRING_T*)&pdir[pentry->NameOffset];
	if(pstr->Length==unamelen&&StringCompare(pstr->NameString, pname, unamelen)==0)
		{
		if(!pentry->DataIsDirectory)
			throw(EINVAL);
		BYTE* proot=(BYTE*)pRoot;
		return (IMAGE_RESOURCE_DIRECTORY*)&proot[pentry->OffsetToDirectory];
		}
	upos+=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY);
	}
throw(EINVAL);
return 0;
}

IMAGE_RESOURCE_DIRECTORY* GetResourceRoot()
{
if(pRoot)
	return pRoot;
MEMORY_BASIC_INFORMATION mbi;
VirtualQuery(&pRoot, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
pBase=(BYTE*)mbi.AllocationBase;
IMAGE_DOS_HEADER* pdos=(IMAGE_DOS_HEADER*)pBase;
if(pdos->e_magic!=IMAGE_DOS_SIGNATURE)
	throw(EINVAL);
IMAGE_NT_HEADERS* pnt=(IMAGE_NT_HEADERS*)&pBase[pdos->e_lfanew];
if(pnt->Signature!=IMAGE_NT_SIGNATURE)
	throw(EINVAL);
UINT ufile=pdos->e_lfanew+4;
UINT usectiondir=ufile+sizeof(IMAGE_FILE_HEADER)+pnt->FileHeader.SizeOfOptionalHeader;
UINT usectionentry=usectiondir;
IMAGE_SECTION_HEADER* prcsection=0;
for(UINT u=0; u<pnt->FileHeader.NumberOfSections; u++)
	{
	IMAGE_SECTION_HEADER* p=(IMAGE_SECTION_HEADER*)&pBase[usectionentry];
	if(StringCompare((LPCSTR)p->Name, ".rsrc", 5)==0)
		{
		prcsection=p;
		break;
		}
	usectionentry+=sizeof(IMAGE_SECTION_HEADER);
	}
if(!prcsection)
	throw(EINVAL);
UINT uroot=prcsection->VirtualAddress;
pRoot=(IMAGE_RESOURCE_DIRECTORY*)&pBase[uroot];
return pRoot;
}


//=========
// Strings
//=========

UINT GetString(IMAGE_RESOURCE_DATA_ENTRY* pdata, UINT uid, LPCWSTR* ppstr)
{
LPCWSTR pblock=(LPCWSTR)&pBase[pdata->OffsetToData];
UINT ublocksize=pdata->Size/sizeof(WCHAR);
UINT ustr=0;
for(UINT ublockpos=0; ublockpos<ublocksize; )
	{
	WCHAR ulen=pblock[ublockpos];
	ublockpos++;
	if(ustr==uid)
		{
		*ppstr=&pblock[ublockpos];
		return ulen;
		}
	ublockpos+=ulen;
	ustr++;
	}
throw(EINVAL);
return 0;
}

LPCWSTR GetResourceString(UINT uid)
{
ASSERT(uid);
IMAGE_RESOURCE_DIRECTORY* proot=GetResourceRoot();
IMAGE_RESOURCE_DIRECTORY* pnamedir=GetResourceDirectory(proot, RC_TYPE_STRING);
UINT ublock=(uid>>4)+1;
IMAGE_RESOURCE_DIRECTORY* plngdir=GetResourceDirectory(pnamedir, ublock);
DWORD ulng=0;
GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_ILANGUAGE|LOCALE_RETURN_NUMBER, (LPWSTR)&ulng, 4);
UINT ublockid=uid&0x0F;
LPCWSTR pstr=0;
UINT ulen=0;
try
	{
	IMAGE_RESOURCE_DATA_ENTRY* pdata=GetResourceData(plngdir, ulng);
	ulen=GetString(pdata, ublockid, &pstr);
	}
catch(UINT uerr)
	{
	if(uerr!=EINVAL)
		throw uerr;
	}
if(!ulen)
	{
	ulng=0;
	IMAGE_RESOURCE_DATA_ENTRY* pdata=GetResourceData(plngdir, ulng);
	ulen=GetString(pdata, ublockid, &pstr);
	}
return pstr;
}


//=======
// Icons
//=======

#pragma pack(push)
#pragma pack(2)

typedef struct ICONGROUPENTRY
{
BYTE Width;
BYTE Height;
BYTE ColorCount;
BYTE Reserved;
WORD Planes;
WORD BitCount;
DWORD Size;
WORD Id;
}ICONGROUPENTRY;

typedef struct ICONGROUP
{
WORD Reserved;
WORD Type;
WORD Count;
ICONGROUPENTRY Entries[1];
}ICONGROUP;

#pragma pack(pop)


BITMAPINFO* GetResourceIcon(UINT uid, UINT usize)
{
ASSERT(uid);
IMAGE_RESOURCE_DIRECTORY* proot=GetResourceRoot();
IMAGE_RESOURCE_DIRECTORY* pnamedir=GetResourceDirectory(proot, RC_TYPE_ICONGROUP);
IMAGE_RESOURCE_DIRECTORY* plngdir=GetResourceDirectory(pnamedir, uid);
DWORD ulng=0;
GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_ILANGUAGE|LOCALE_RETURN_NUMBER, (LPWSTR)&ulng, 4);
IMAGE_RESOURCE_DATA_ENTRY* pdata=0;
try
	{
	pdata=GetResourceData(plngdir, ulng);
	}
catch(UINT uerr)
	{
	if(uerr!=EINVAL)
		throw uerr;
	ulng=0;
	pdata=GetResourceData(plngdir, ulng);
	}
ICONGROUP* picongroup=(ICONGROUP*)&pBase[pdata->OffsetToData];
ICONGROUPENTRY* picongroupentries=picongroup->Entries;
UINT uicocount=picongroup->Count;
UINT uico=0;
for(; uico<uicocount; uico++)
	{
	if(!usize||picongroupentries[uico].Width==usize)
		break;
	}
if(uico==uicocount)
	uico=0;
pnamedir=GetResourceDirectory(proot, RC_TYPE_ICON);
plngdir=GetResourceDirectory(pnamedir, picongroupentries[uico].Id);
pdata=GetResourceData(plngdir, ulng);
return (BITMAPINFO*)&pBase[pdata->OffsetToData];
}
