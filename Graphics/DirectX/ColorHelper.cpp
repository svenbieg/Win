//=================
// ColorHelper.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "ColorHelper.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//========
// Common
//========

#ifdef __cplusplus_winrt
COLOR ColorFromUIColor(Windows::UI::Color c)
{
UINT r=c.R;
UINT g=c.G;
UINT b=c.B;
UINT a=c.A;
return (a<<24)|(b<<16)|(g<<8)|r;
}
#endif

COLOR ColorToValue(FLOAT4 const& c)
{
UINT r=(UINT)(c.x/255.f);
UINT g=(UINT)(c.y/255.f);
UINT b=(UINT)(c.z/255.f);
UINT a=(UINT)(c.w/255.f);
return (a<<24)|(b<<16)|(g<<8)|r;
}

FLOAT4 ColorToVector(COLOR c)
{
FLOAT4 v;
v.x=(FLOAT)((UINT)c&0xFF)/255.f;
v.y=(FLOAT)(((UINT)c>>8)&0xFF)/255.f;
v.z=(FLOAT)(((UINT)c>>16)&0xFF)/255.f;
v.w=(FLOAT)((UINT)c>>24)/255.f;
return v;
}

}}