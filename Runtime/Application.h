//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "Core/Application.h"
#include "Storage/Collections/Log.h"
#include "Storage/Collections/Settings.h"
#include "Storage/AppData.h"


//===========
// Namespace
//===========

namespace Runtime {


//=============
// Application
//=============

class Application: public Core::Application
{
public:
	// Common
	Handle<Storage::AppData> AppData;
	static Handle<Application> Current;
	Handle<Storage::Collections::Log> Log;
	Handle<Storage::Collections::Settings> Settings;

protected:
	// Con-/Destructors
	Application(Handle<String> Name, Handle<String> UpdateUrl=nullptr);
};

}