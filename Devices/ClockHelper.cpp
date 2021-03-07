//=================
// ClockHelper.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <time.h>
#include "ClockHelper.h"

using namespace Physics;


//===========
// Namespace
//===========

namespace Devices {


//========
// Common
//========

BOOL ClockGetTime(TIMEPOINT& tp, UINT udelta)
{
time_t tnow;
time(&tnow);
tnow-=udelta;
tm tmnow;
localtime_s(&tmnow, &tnow);
if(tmnow.tm_year+1900<2000)
	return false;
tp.Second=tmnow.tm_sec;
tp.Minute=tmnow.tm_min;
tp.Hour=tmnow.tm_hour;
tp.DayOfWeek=tmnow.tm_wday;
tp.DayOfMonth=tmnow.tm_mday;
tp.Month=tmnow.tm_mon;
tp.Year=tmnow.tm_year;
return true;
}

}