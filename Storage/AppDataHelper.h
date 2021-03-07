//=================
// AppDataHelper.h
//=================

#pragma once


//=======
// Using
//=======

#include "Storage/Directory.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

Handle<Directory> GetAppDataLocal();
Handle<String> GetAppDataLocalPath();
Handle<Directory> GetAppDataNetwork();
Handle<String> GetAppDataNetworkPath();

}