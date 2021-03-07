//===================
// AppDataHelper.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include "Core/Application.h"
#include "Storage/Filesystem/Directory.h"
#include "AppDataHelper.h"

using namespace Core;


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<Directory> GetAppDataLocal()
{
auto path=GetAppDataLocalPath();
if(!path)
	return nullptr;
return new Filesystem::Directory("Local", path);
}

Handle<String> GetAppDataLocalPath()
{
auto app=Application::Current;
if(!app)
	return nullptr;
auto name=app->Name;
if(!name)
	return nullptr;
TCHAR path[MAX_PATH];
GetEnvironmentVariable(TEXT("LOCALAPPDATA"), path, MAX_PATH);
return new String("%s\\%s", path, name->Begin());
}

Handle<Directory> GetAppDataNetwork()
{
auto path=GetAppDataNetworkPath();
if(!path)
	return nullptr;
return new Filesystem::Directory("Network", path);
}

Handle<String> GetAppDataNetworkPath()
{
auto app=Application::Current;
if(!app)
	return nullptr;
auto name=app->Name;
if(!name)
	return nullptr;
TCHAR path[MAX_PATH];
GetEnvironmentVariable(TEXT("HOMEPATH"), path, MAX_PATH);
return new String("%s\\AppData\\Network\\%s", path, name->Begin());
}

}