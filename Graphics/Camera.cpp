//============
// Camera.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Camera.h"
#include "Scene.h"

using namespace DirectX;


//===========
// Namespace
//===========

namespace Graphics {


//===========
// Constants
//===========

const FLOAT3 vFwd={ 0.f, 0.f, -1.f };
const FLOAT3 vUp={ 0.f, 1.f, 0.f };


//==================
// Con-/Destructors
//==================

Camera::Camera(Handle<Scene> hscene):
hScene(hscene),
vPosition(0.f, 0.f, 1.f),
vRotation(0.f, 0.f, 0.f, 1.f)
{
ASSERT(hscene);
SetFlag(uFlags, CameraFlags::MatrixInvalid);
}


//========
// Common
//========

VOID Camera::MoveAway(Handle<DeviceContext> hdc)
{
VECTOR vpos=XMLoadFloat3(&vPosition);
VECTOR vnull=XMVectorNegate(vpos);
FLOAT fnull=XMVector3Length(vnull).M128_F32[0];
VECTOR vrot=XMLoadFloat4(&vRotation);
VECTOR vfwd=XMVector3Rotate(XMLoadFloat3(&vFwd), vrot);
FLOAT fa=XMVector3AngleBetweenVectors(vnull, vfwd).M128_F32[0];
FLOAT fd=sinf(fa)*fnull;
FLOAT fold=cosf(fa)*fnull;
FLOAT flen=hScene->GetRadius()+fd;
MATRIX mxproj=hdc->GetProjectionMatrix();
FLOAT fov=atanf(1/mxproj.r[1].M128_F32[1])*180.f/XM_PI;
FLOAT fx=flen/(FLOAT)tan(fov);
FLOAT fnew=fx-fold;
FLOAT3 vnew(fnew, fnew, fnew);
VECTOR vback=XMVectorNegate(vfwd);
VECTOR vmove=XMVectorMultiply(vback, XMLoadFloat3(&vnew));
vpos=XMVectorAdd(vpos, vmove);
XMStoreFloat3(&vPosition, vpos);
}


//================
// Common Private
//================

VOID Camera::UpdateMatrix()
{
VECTOR vpos=XMLoadFloat3(&vPosition);
VECTOR vrot=XMLoadFloat4(&vRotation);
VECTOR vfwd=XMVector3Rotate(XMLoadFloat3(&vFwd), vrot);
VECTOR vup=XMVector3Rotate(XMLoadFloat3(&vUp), vrot);
VECTOR vat=XMVectorAdd(vpos, vfwd);
MATRIX mxview=XMMatrixTranspose(XMMatrixLookAtRH(vpos, vat, vup));
XMStoreFloat4x4(&mxView, mxview);
}

}
