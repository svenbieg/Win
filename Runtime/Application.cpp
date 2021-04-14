//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Application.h"


//===========
// Namespace
//===========

namespace Runtime {


//========
// Common
//========

Handle<Application> Application::Current;


//============================
// Con-/Destructors Protected
//============================

Application::Application(Handle<String> hname, Handle<String> hupdateurl):
Core::Application(hname)
{
Current=this;
AppData=new Storage::AppData();
Log=new Storage::Collections::Log();
Settings=new Storage::Collections::Settings();
}

}