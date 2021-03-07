//===============
// ColorHelper.h
//===============

#pragma once


//=======
// Using
//=======

#include "Graphics/Color.h"
#include "Version.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//========
// Common
//========

#ifdef __cplusplus_winrt
COLOR ColorFromUIColor(Windows::UI::Color Color);
#endif

COLOR ColorToValue(FLOAT4 const& Vector);
FLOAT4 ColorToVector(COLOR Color);

}}