//==========
// Camera.h
//==========

#pragma once


//=======
// Using
//=======

#include "DirectX/DeviceContext.h"


//===========
// Namespace
//===========

namespace Graphics {


//=======
// Flags
//=======

enum class CameraFlags: UINT
{
MatrixInvalid=1
};


//======================
// Forward-Declarations
//======================

class Scene;


//========
// Camera
//========

class Camera: public Object
{
private:
	// Using
	using DeviceContext=DirectX::DeviceContext;

public:
	// Con-/Destructors
	Camera(Handle<Scene> Scene);

	// Common
	FLOAT3 const& GetPosition()const { return vPosition; }
	FLOAT4 const& GetRotation()const { return vRotation; }
	Handle<Scene> GetScene() { return hScene; }
	inline MATRIX GetViewMatrix() { if(GetFlag(uFlags, CameraFlags::MatrixInvalid))UpdateMatrix(); return ::DirectX::XMLoadFloat4x4(&mxView); }
	VOID MoveAway(Handle<DeviceContext> DeviceContext);
	VOID SetPosition(FLOAT3 const& Position) { vPosition=Position; SetFlag(uFlags, CameraFlags::MatrixInvalid); }
	VOID SetRotation(FLOAT4 const& Rotation) { vRotation=Rotation; SetFlag(uFlags, CameraFlags::MatrixInvalid); }

private:
	// Common
	VOID UpdateMatrix();
	Handle<Scene> hScene;
	FLOAT4X4 mxView;
	UINT uFlags;
	FLOAT3 vPosition;
	FLOAT4 vRotation;
};

}
