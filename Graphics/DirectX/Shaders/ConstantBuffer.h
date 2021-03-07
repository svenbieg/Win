//==================
// ConstantBuffer.h
//==================

#pragma once


//=======
// Using
//=======

#include "Graphics/DirectX/Version.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {
		namespace Shaders {


//========
// Struct
//========

typedef struct
{
MATRIX ModelMatrix;
MATRIX ViewMatrix;
MATRIX ProjectionMatrix;
FLOAT4 AmbientColor;
FLOAT4 Color;
}CONSTANT_BUFFER;

}}}
