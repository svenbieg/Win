//=============
// Texture.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "DirectX/DeviceContext.h"
#include "Texture.h"


//===========
// Namespace
//===========

namespace Graphics {


//==================
// Con-/Destructors
//==================

Texture::Texture(Handle<Bitmap> hbmp, UINT umip):
hBitmap(hbmp),
uMipLevels(umip)
{
hTextures=new DeviceMap();
}

Texture::~Texture()
{
for(auto hit=hTextures->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto pdc=hit->GetCurrentId();
	pdc->Destroyed.Remove(this);
	pdc->DeviceLost.Remove(this);
	}
}


//========
// Common
//========

D3D_SHADER_RESOURCE_VIEW* Texture::GetView(Handle<DeviceContext> hdc)
{
ScopedLock lock(cCriticalSection);
ComPtr<D3D_SHADER_RESOURCE_VIEW> pview;
if(hTextures->TryGet(hdc, &pview))
	{
	if(pview.Get())
		return pview.Get();
	}
Microsoft::WRL::ComPtr<D3D_SHADER_RESOURCE_VIEW> pnew;
CreateView(hdc, pnew.GetAddressOf());
pview=pnew.Get();
hTextures->Set(hdc, pview);
hdc->Destroyed.Add(this, &Texture::OnDeviceContextDestroyed);
hdc->DeviceLost.Add(this, &Texture::OnDeviceLost);
return pview.Get();
}


//================
// Common Private
//================

VOID Texture::CreateView(Handle<DeviceContext> hdc, D3D_SHADER_RESOURCE_VIEW** ppview)
{
UINT uwidth=hBitmap->GetWidth();
UINT uheight=hBitmap->GetHeight();
D3D_TEXTURE_DESC dsc;
ZeroMemory(&dsc, sizeof(D3D_TEXTURE_DESC));
dsc.Width=uwidth;
dsc.Height=uheight;
dsc.MipLevels=uMipLevels;
dsc.ArraySize=1;
dsc.Format=SWAPCHAIN_FORMAT;
dsc.SampleDesc.Count=1;
dsc.Usage=D3D11_USAGE_DEFAULT;
dsc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
D3D_DEVICE* pdev=hdc->GetD3DDevice();
Microsoft::WRL::ComPtr<D3D_TEXTURE> ptexture;
ThrowIfFailed(pdev->CreateTexture2D(&dsc, 0, ptexture.GetAddressOf()));
D3D_DEVICE_CONTEXT* pdc=hdc->GetD3DDeviceContext();
auto hbuf=hBitmap->GetBuffer();
pdc->UpdateSubresource(ptexture.Get(), 0, nullptr, hbuf->Begin(), hBitmap->GetPitch(), hBitmap->GetSize());
if(uMipLevels>1)
	{
	auto hbmp=hBitmap;
	for(UINT u=1; u<uMipLevels; u++)
		{
		uwidth/=2;
		uheight/=2;
		hbmp=hbmp->Scale(hdc, uwidth, uheight);
		auto hbuf=hbmp->GetBuffer();
		pdc->UpdateSubresource(ptexture.Get(), u, nullptr, hbuf->Begin(), hbmp->GetPitch(), hbmp->GetSize());
		}
	}
ThrowIfFailed(pdev->CreateShaderResourceView(ptexture.Get(), 0, ppview));
}

VOID Texture::OnDeviceContextDestroyed(DeviceContext* pdc)
{
RemoveDevice(pdc);
}

VOID Texture::OnDeviceLost(Handle<DeviceContext> hdc)
{
RemoveDevice(hdc);
}

VOID Texture::RemoveDevice(DeviceContext* pdc)
{
ScopedLock lock(cCriticalSection);
auto hit=hTextures->Find(pdc);
if(!hit->HasCurrent())
	return;
if(hit->GetCurrentId()!=pdc)
	return;
pdc->Destroyed.Remove(this);
pdc->DeviceLost.Remove(this);
hit->RemoveCurrent();
}

}