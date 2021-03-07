//=========
// Scene.h
//=========

#pragma once


//=======
// Using
//=======

#include "Collections/Index.h"
#include "Camera.h"
#include "Model.h"


//===========
// Namespace
//===========

namespace Graphics {


//=======
// Scene
//=======

class Scene: public Object
{
private:
	// Using
	using DeviceContext=DirectX::DeviceContext;
	using ModelIndex=Collections::Index<Handle<Model>>;

public:
	// Con-/Destructors
	Scene();

	// Common
	FLOAT4 const& GetAmbientColor() { return vAmbientColor; }
	Handle<Graphics::Camera> Camera;
	VOID Draw(Handle<DeviceContext> DeviceContext);
	FLOAT GetRadius();
	Handle<ModelIndex> Models;

private:
	// Common
	VOID OnModelsChanged(Handle<ModelIndex> Index);
	FLOAT fRadius;
	FLOAT4 vAmbientColor;
};

}
