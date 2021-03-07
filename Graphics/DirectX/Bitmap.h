//==========
// Bitmap.h
//==========

#pragma once


//=======
// Using
//=======

#include "Collections/Map.h"
#include "Storage/Buffer.h"
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


//========
// Bitmap
//========

class Bitmap: public Object
{
private:
	// Using
	using DeviceMap=Collections::Map<DeviceContext*, ComPtr<D2D_BITMAP>>;
	using Buffer=Storage::Buffer;

public:
	// Con-/Destructors
	Bitmap(UINT Id, UINT Size);
	Bitmap(UINT Width, UINT Height, BYTE const* Buffer);
	Bitmap(Handle<DeviceContext> DeviceContext, Handle<String> Path);
	Bitmap(Handle<DeviceContext> DeviceContext, BYTE const* Buffer, UINT Size);
	~Bitmap();

	// Common
	Handle<Buffer> GetBuffer()const { return hBuffer; }
	VOID Draw(Handle<DeviceContext> DeviceContext);
	VOID Draw(Handle<DeviceContext> DeviceContext, FLOAT X, FLOAT Y, FLOAT Width, FLOAT Height);
	D2D_BITMAP* GetBitmap(Handle<DeviceContext> DeviceContext);
	UINT GetHeight()const { return uHeight; }
	Handle<String> GetPath() { return hPath; }
	UINT GetPitch() { return uPitch; }
	UINT GetSize()const { return hBuffer? (UINT)hBuffer->GetSize(): 0; }
	UINT GetWidth()const { return uWidth; }

	// Modification
	Handle<Bitmap> Scale(Handle<DeviceContext> DeviceContext, UINT Width, UINT Height);

private:
	// Common
	VOID CreateBitmap(Handle<DeviceContext> DeviceContext, D2D_BITMAP** Bitmap);
	VOID OnDeviceContextDestroyed(DeviceContext* DeviceContext);
	VOID OnDeviceLost(Handle<DeviceContext> DeviceContext);
	VOID OpenStream(Handle<DeviceContext> DeviceContext, WIC_FACTORY* Factory, WIC_STREAM* Stream);
	VOID RemoveDevice(DeviceContext* DeviceContext);
	Handle<DeviceMap> hBitmaps;
	Handle<Buffer> hBuffer;
	Handle<String> hPath;
	UINT uHeight;
	UINT uPitch;
	UINT uWidth;
};

}}