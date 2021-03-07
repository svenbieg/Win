//==============
// VertexList.h
//==============

#pragma once


//=======
// Using
//=======

#include "Collections/Index.h"
#include "Version.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//======================
// Forward-Declarations
//======================

class DeviceContext;


//=============
// Vertex-List
//=============

class VertexList: public Object
{
private:
	// Device-List
	typedef struct
		{
		ComPtr<D3D_BUFFER> IndexBuffer;
		ComPtr<D3D_BUFFER> VertexBuffer;
		}VERTEXLIST;

	// Using
	using DeviceMap=Collections::Map<DeviceContext*, VERTEXLIST>;

public:
	// Con-/Destructors
	VertexList();
	~VertexList() { Destroy(); }
	VOID Destroy();

	// Common
	VOID AddTriangle(VERTEX const& V1, VERTEX const& V2, VERTEX const& V3);
	VOID AddQuad(VERTEX const& V1, VERTEX const& V2, VERTEX const& V3, VERTEX const& V4);
	VOID Draw(Handle<DeviceContext> DeviceContext, D3D_PRIMITIVE_TOPOLOGY Topology=D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT GetIndexCount() { return uIndexCount; }
	UINT const* GetIndices() { return pIndices; }
	UINT GetVertexCount() { return uVertexCount; }
	VERTEX const* GetVertices() { return pVertices; }

private:
	// Common
	VOID CreateList(Handle<DeviceContext> DeviceContext, VERTEXLIST* List);
	VOID DestroyVertexLists();
	VERTEXLIST GetList(Handle<DeviceContext> DeviceContext, BOOL Create);
	VOID OnDeviceContextDestroyed(DeviceContext* DeviceContext);
	VOID OnDeviceLost(Handle<DeviceContext> DeviceContext);
	VOID RemoveDevice(DeviceContext* DeviceContext);
	VOID SetIndexSize(UINT Size);
	VOID SetVertexSize(UINT Size);
	CriticalSection cCriticalSection;
	Handle<DeviceMap> hLists;
	UINT* pIndices;
	VERTEX* pVertices;
	UINT uIndexCount;
	UINT uVertexCount;
};

}}