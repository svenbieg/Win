//===========
// Scene.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "DirectX/DeviceContext.h"
#include "Scene.h"

using namespace DirectX;


//===========
// Namespace
//===========

namespace Graphics {


//==================
// Con-/Destructors
//==================

Scene::Scene():
vAmbientColor({ 1.f, 1.f, 1.f, 1.f })
{
Camera=new Graphics::Camera(this);
Models=new ModelIndex();
Models->Changed.Add(this, &Scene::OnModelsChanged);
}


//========
// Common
//========

VOID Scene::Draw(Handle<DeviceContext> hdc)
{
hdc->SetAmbientColor(vAmbientColor);
for(auto hit=Models->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto hmodel=hit->GetCurrent();
	hmodel->Draw(hdc);
	}
}

FLOAT Scene::GetRadius()
{
if(fRadius>=0.f)
	return fRadius;
fRadius=0.f;
for(auto hit=Models->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto hmodel=hit->GetCurrent();
	VECTOR vpos=XMLoadFloat3(&hmodel->GetPosition());
	VECTOR vnormal=XMVector3Normalize(vpos);
	if(XMVector3Length(vnormal).M128_F32[0]==0.f)
		vnormal.M128_F32[0]=1.f;
	FLOAT fradius=hmodel->GetRadius();
	MATRIX mxscale=XMMatrixScaling(fradius, fradius, fradius);
	VECTOR vmodel=XMVector3Transform(vnormal, mxscale);
	VECTOR vscene=XMVectorAdd(vpos, vmodel);
	VECTOR vradius=XMVector3Length(vscene);
	fRadius=max(fRadius, vradius.M128_F32[0]);
	}
return fRadius;
}


//================
// Common Private
//================

VOID Scene::OnModelsChanged(Handle<ModelIndex> hmodels)
{
fRadius=-1.f;
}

}
