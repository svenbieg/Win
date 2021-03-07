//=========
// Model.h
//=========

#pragma once


//=======
// Using
//=======

#include "DirectX/Brush.h"
#include "DirectX/VertexList.h"
#include "Material.h"


//===========
// Namespace
//===========

namespace Graphics {


//=======
// Flags
//=======

enum class ModelFlags: UINT
{
None=0,
MatrixInvalid=1
};


//=======
// Model
//=======

class Model: public Object
{
private:
	// Using
	using Brush=DirectX::Brush;
	using DeviceContext=DirectX::DeviceContext;
	using VertexList=DirectX::VertexList;

public:
	// Con-/Destructors
	Model(Handle<VertexList> VertexList, Handle<Brush> Color, Handle<Material> Material);

	// Common
	Handle<Brush> Color;
	VOID Draw(Handle<DeviceContext> DeviceContext);
	MATRIX GetMatrix();
	FLOAT3 const& GetPosition() { return vPosition; }
	FLOAT GetRadius();
	FLOAT4 const& GetRotation() { return vRotation; }
	FLOAT3 const& GetScale() { return vScale; }
	Handle<VertexList> GetVertexList() { return hVertexList; }
	Handle<Material> Material;
	Event<Model> PointerMoved;
	Event<Model> PointerPressed;
	Event<Model> PointerReleased;
	Event<Model> PointerWheelChanged;
	Event<Model> RightTapped;
	VOID SetPosition(FLOAT3 const& Position) { vPosition=Position; SetFlag(uFlags, ModelFlags::MatrixInvalid); }
	VOID SetRotation(FLOAT4 const& Rotation) { vRotation=Rotation; SetFlag(uFlags, ModelFlags::MatrixInvalid); }
	VOID SetScale(FLOAT3 const& Scale) { vScale=Scale; SetFlag(uFlags, ModelFlags::MatrixInvalid); fRadius=-1.f; }
	Event<Model> Tapped;

private:
	// Common
	FLOAT fRadius;
	Handle<VertexList> hVertexList;
	FLOAT4X4 mxModel;
	ModelFlags uFlags;
	FLOAT3 vPosition;
	FLOAT4 vRotation;
	FLOAT3 vScale;
};

}
