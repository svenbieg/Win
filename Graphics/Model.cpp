//===========
// Model.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "DirectX/DeviceContext.h"
#include "Model.h"

using namespace DirectX;


//===========
// Namespace
//===========

namespace Graphics {


//==================
// Con-/Destructors
//==================

Model::Model(Handle<VertexList> hvertices, Handle<Brush> hbrush, Handle<Graphics::Material> hmat):
Color(hbrush),
Material(hmat),
hVertexList(hvertices),
fRadius(-1.f),
uFlags(ModelFlags::MatrixInvalid),
vPosition({ 0.f, 0.f, 0.f }),
vRotation({ 0.f, 0.f, 0.f, 0.f }),
vScale({ 1.f, 1.f, 1.f })
{
if(!Color)
	Color=new Brush(1.f, 1.f, 1.f, 1.f);
}


//========
// Common
//========

VOID Model::Draw(Handle<DeviceContext> hdc)
{
MATRIX mx=GetMatrix();
hdc->SetModelMatrix(mx);
hdc->FlushConstantBuffer();
hdc->SetMaterial(Material);
hVertexList->Draw(hdc);
}

MATRIX Model::GetMatrix()
{
if(GetFlag(uFlags, ModelFlags::MatrixInvalid))
	{
	MATRIX mxpos=XMMatrixTranslationFromVector(XMLoadFloat3(&vPosition));
	MATRIX mxrot=XMMatrixRotationQuaternion(XMLoadFloat4(&vRotation));
	MATRIX mxscale=XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	MATRIX mxmodel=XMMatrixMultiply(mxpos, mxrot);
	mxmodel=XMMatrixMultiply(mxmodel, mxscale);
	mxmodel=XMMatrixTranspose(mxmodel);
	XMStoreFloat4x4(&mxModel, mxmodel);
	}
return ::DirectX::XMLoadFloat4x4(&mxModel);
}

FLOAT Model::GetRadius()
{
if(fRadius>=0.f)
	return fRadius;
fRadius=0.f;
MATRIX mxscale=XMMatrixScaling(vScale.x, vScale.y, vScale.z);
UINT ucount=hVertexList->GetVertexCount();
auto pvertices=hVertexList->GetVertices();
for(UINT u=0; u<ucount; u++)
	{
	VECTOR v=XMLoadFloat3(&pvertices[u].Position);
	v=XMVector3Transform(v, mxscale);
	v=XMVector3Length(v);
	fRadius=max(fRadius, v.M128_F32[0]);
	}
return fRadius;
}

}
