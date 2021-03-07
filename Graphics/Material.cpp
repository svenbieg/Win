//==============
// Material.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Material.h"


//===========
// Namespace
//===========

namespace Graphics {


//==================
// Con-/Destructors
//==================

Material::Material(Handle<Graphics::Texture> htexture, Handle<Graphics::Texture> hspecmap):
SpecularMap(hspecmap),
Texture(htexture)
{}


//========
// Common
//========

VOID Material::Clear()
{
SpecularMap=nullptr;
Texture=nullptr;
}

}
