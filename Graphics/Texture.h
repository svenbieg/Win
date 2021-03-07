//===========
// Texture.h
//===========

#pragma once


//=======
// Using
//=======

#include "DirectX/Bitmap.h"


//===========
// Namespace
//===========

namespace Graphics {


//=========
// Texture
//=========

class Texture: public Object
{
private:
	// Using
	using Bitmap=DirectX::Bitmap;
	using DeviceContext=DirectX::DeviceContext;
	using DeviceMap=Collections::Map<DeviceContext*, ComPtr<D3D_SHADER_RESOURCE_VIEW>>;

public:
	// Con-/Destructors
	Texture(Handle<Bitmap> Bitmap, UINT MipLevels=1);
	~Texture();

	// Common
	Handle<Bitmap> GetBitmap()const { return hBitmap; }
	UINT GetMipLevels()const { return uMipLevels; }
	D3D_SHADER_RESOURCE_VIEW* GetView(Handle<DeviceContext> DeviceContext);

private:
	// Common
	VOID CreateView(Handle<DeviceContext> DeviceContext, D3D_SHADER_RESOURCE_VIEW** View);
	VOID OnDeviceContextDestroyed(DeviceContext* DeviceContext);
	VOID OnDeviceLost(Handle<DeviceContext> DeviceContext);
	VOID RemoveDevice(DeviceContext* DeviceContext);
	CriticalSection cCriticalSection;
	Handle<Bitmap> hBitmap;
	Handle<DeviceMap> hTextures;
	UINT uMipLevels;
};

}
