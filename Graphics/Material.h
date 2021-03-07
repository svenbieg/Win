//============
// Material.h
//============

#pragma once


//=======
// Using
//=======

#include "Texture.h"


//===========
// Namespace
//===========

namespace Graphics {


//==========
// Material
//==========

class Material: public Object
{
public:
	// Con-/Destructors
	Material(Handle<Graphics::Texture> Texture=nullptr, Handle<Graphics::Texture> SpecularMap=nullptr);

	// Common
	VOID Clear();
	Handle<Graphics::Texture> SpecularMap;
	Handle<Graphics::Texture> Texture;
};

}
