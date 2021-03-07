//================
// VertexList.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "DeviceContext.h"
#include "VertexList.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//==================
// Con-/Destructors
//==================

VertexList::VertexList():
pIndices(nullptr),
pVertices(nullptr),
uIndexCount(0),
uVertexCount(0)
{
hLists=new DeviceMap();
}

VOID VertexList::Destroy()
{
ScopedLock lock(cCriticalSection);
DestroyVertexLists();
if(pIndices)
	AlignedFree(pIndices);
pIndices=nullptr;
uIndexCount=0;
if(pVertices)
	AlignedFree(pVertices);
pVertices=nullptr;
uVertexCount=0;
}


//========
// Common
//========

VOID VertexList::AddTriangle(const VERTEX& v1, const VERTEX& v2, const VERTEX& v3)
{
ScopedLock lock(cCriticalSection);
DestroyVertexLists();
SetIndexSize(uIndexCount+3);
for(UINT u=0; u<3; u++)
	pIndices[uIndexCount+u]=uVertexCount+u;
uIndexCount+=3;
SetVertexSize(uVertexCount+3);
CopyMemory(&pVertices[uVertexCount], &v1, sizeof(VERTEX));
CopyMemory(&pVertices[uVertexCount+1], &v2, sizeof(VERTEX));
CopyMemory(&pVertices[uVertexCount+2], &v3, sizeof(VERTEX));
uVertexCount+=3;
}

VOID VertexList::AddQuad(const VERTEX& v1, const VERTEX& v2, const VERTEX& v3, const VERTEX& v4)
{
ScopedLock lock(cCriticalSection);
DestroyVertexLists();
SetIndexSize(uIndexCount+6);
pIndices[uIndexCount]=uVertexCount;
pIndices[uIndexCount+1]=uVertexCount+1;
pIndices[uIndexCount+2]=uVertexCount+2;
pIndices[uIndexCount+3]=uVertexCount+3;
pIndices[uIndexCount+4]=uVertexCount;
pIndices[uIndexCount+5]=uVertexCount+2;
uIndexCount+=6;
SetVertexSize(uVertexCount+4);
CopyMemory(&pVertices[uVertexCount], &v1, sizeof(VERTEX));
CopyMemory(&pVertices[uVertexCount+1], &v2, sizeof(VERTEX));
CopyMemory(&pVertices[uVertexCount+2], &v3, sizeof(VERTEX));
CopyMemory(&pVertices[uVertexCount+3], &v4, sizeof(VERTEX));
uVertexCount+=4;
}

VOID VertexList::Draw(Handle<DeviceContext> hdc, D3D_PRIMITIVE_TOPOLOGY topology)
{
auto pdc=hdc->GetD3DDeviceContext();
UINT uvsize=sizeof(VERTEX);
UINT uvoffset=0;
	{
	ScopedLock lock(cCriticalSection);
	auto list=GetList(hdc, true);
	pdc->IASetVertexBuffers(0, 1, list.VertexBuffer.GetAddressOf(), &uvsize, &uvoffset);
	pdc->IASetIndexBuffer(list.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
pdc->IASetPrimitiveTopology(topology);
pdc->DrawIndexed(uIndexCount, 0, 0);
}


//================
// Common Private
//================

VOID VertexList::CreateList(Handle<DeviceContext> hdc, VERTEXLIST* plist)
{
hdc->Destroyed.Add(this, &VertexList::OnDeviceContextDestroyed);
hdc->DeviceLost.Add(this, &VertexList::OnDeviceLost);
D3D_DEVICE* pdev=hdc->GetD3DDevice();
D3D_BUFFER_DESC cdsc(uVertexCount*sizeof(VERTEX), D3D11_BIND_VERTEX_BUFFER);
D3D_SUBRESOURCE_DATA csrd;
ZeroMemory(&csrd, sizeof(D3D_SUBRESOURCE_DATA));
csrd.pSysMem=pVertices;
Microsoft::WRL::ComPtr<D3D_BUFFER> pvb;
ThrowIfFailed(pdev->CreateBuffer(&cdsc, &csrd, &pvb));
plist->VertexBuffer=pvb.Get();
cdsc.BindFlags=D3D11_BIND_INDEX_BUFFER;
cdsc.ByteWidth=uIndexCount*sizeof(UINT);
csrd.pSysMem=pIndices;
Microsoft::WRL::ComPtr<D3D_BUFFER> pib;
ThrowIfFailed(pdev->CreateBuffer(&cdsc, &csrd, &pib));
plist->IndexBuffer=pib.Get();
}

VOID VertexList::DestroyVertexLists()
{
for(auto hit=hLists->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto hdc=hit->GetCurrentId();
	hdc->Destroyed.Remove(this);
	hdc->DeviceLost.Remove(this);
	auto list=hit->GetCurrentItem();
	list.IndexBuffer=nullptr;
	list.VertexBuffer=nullptr;
	}
hLists->Clear();
}

VertexList::VERTEXLIST VertexList::GetList(Handle<DeviceContext> hdc, BOOL bcreate)
{
VERTEXLIST list;
if(hLists->TryGet(hdc, &list))
	{
	if(!list.IndexBuffer&&bcreate)
		{
		CreateList(hdc, &list);
		hLists->Set(hdc, list);
		}
	return list;
	}
if(!bcreate)
	return list;
CreateList(hdc, &list);
hLists->Set(hdc, list);
return list;
}

VOID VertexList::OnDeviceContextDestroyed(DeviceContext* pdc)
{
RemoveDevice(pdc);
}

VOID VertexList::OnDeviceLost(Handle<DeviceContext> hdc)
{
RemoveDevice(hdc);
}

VOID VertexList::RemoveDevice(DeviceContext* pdc)
{
ScopedLock lock(cCriticalSection);
VERTEXLIST list;
if(hLists->TryGet(pdc, &list))
	{
	pdc->Destroyed.Remove(this);
	pdc->DeviceLost.Remove(this);
	hLists->Remove(pdc);
	}
}

VOID VertexList::SetIndexSize(UINT usize)
{
if(!pIndices)
	{
	pIndices=(UINT*)AlignedAlloc(usize*sizeof(UINT), 16);
	}
else
	{
	pIndices=(UINT*)AlignedRealloc(pIndices, usize*sizeof(UINT), 16);
	}
}

VOID VertexList::SetVertexSize(UINT usize)
{
if(!pVertices)
	{
	pVertices=(VERTEX*)AlignedAlloc(usize*sizeof(VERTEX), 16);
	}
else
	{
	pVertices=(VERTEX*)AlignedRealloc(pVertices, usize*sizeof(VERTEX), 16);
	}
}

}}