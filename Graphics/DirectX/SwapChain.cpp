//===============
// SwapChain.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "DeviceContext.h"
#include "SwapChain.h"

using namespace D2D1;
using namespace DirectX;

#ifdef __cplusplus_winrt
#include <windows.ui.xaml.media.dxinterop.h>
using namespace Windows::UI::Xaml::Controls;
#endif


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//==========
// Rotation
//==========

XMMATRIX mxRotation0(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

XMMATRIX mxRotation90(
	0.0f, 1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

XMMATRIX mxRotation180(
	-1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);

XMMATRIX mxRotation270(
	0.0f, -1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f);


//==================
// Con-/Destructors
//==================

#ifndef __cplusplus_winrt
SwapChain::SwapChain(Handle<DeviceContext> hdc, HWND hwnd, UINT udpi):
hDeviceContext(hdc),
hWindow(hwnd),
mxOrientation(mxRotation0),
uDpi(udpi),
uRotation(DXGI_MODE_ROTATION_IDENTITY)
{
Initialize();
}
#endif

#ifdef __cplusplus_winrt
SwapChain::SwapChain(Handle<DeviceContext> hdc, SwapChainPanel^ hpanel, UINT udpi, DXGI_MODE_ROTATION urot):
hDeviceContext(hdc),
mxOrientation(mxRotation0),
uDpi(udpi),
uRotation(DXGI_MODE_ROTATION_IDENTITY),
pPanel(reinterpret_cast<IUnknown*>(hpanel))
{
SetRotationInternal(urot);
Initialize();
}
#endif


//========
// Common
//========

Matrix3x2F SwapChain::GetOrientationMatrix2D()
{
Matrix3x2F mxscale;
ZeroMemory(&mxscale, sizeof(Matrix3x2F));
mxscale._11=96.f/uDpi;
mxscale._22=96.f/uDpi;
switch(uRotation)
	{
	case DXGI_MODE_ROTATION_IDENTITY:
		return mxscale;
	case DXGI_MODE_ROTATION_ROTATE90:
		return mxscale*Matrix3x2F::Rotation(90.f)*Matrix3x2F::Translation((FLOAT)cPanelSize.Height, 0.f);
	case DXGI_MODE_ROTATION_ROTATE180:
		return mxscale*Matrix3x2F::Rotation(180.f)*Matrix3x2F::Translation((FLOAT)cPanelSize.Width, (FLOAT)cPanelSize.Height);
	case DXGI_MODE_ROTATION_ROTATE270:
		return mxscale*Matrix3x2F::Rotation(270.f)*Matrix3x2F::Translation(0.f, (FLOAT)cPanelSize.Width);
	}
return mxscale;
}

VOID SwapChain::Present()
{
if(!pSwapChain)
	return;
DXGI_PRESENT_PARAMETERS params;
ZeroMemory(&params, sizeof(DXGI_PRESENT_PARAMETERS));
pSwapChain->Present1(1, 0, &params);
}

VOID SwapChain::SetRotation(DXGI_MODE_ROTATION urot)
{
if(urot==uRotation)
	return;
SetRotationInternal(urot);
Update();
}

VOID SwapChain::Set(UINT uwidth, UINT uheight, UINT udpi, DXGI_MODE_ROTATION urot)
{
cPanelSize.Width=uwidth;
cPanelSize.Height=uheight;
uDpi=udpi;
SetRotationInternal(urot);
Update();
}

VOID SwapChain::SetDotsPerInch(UINT udpi)
{
if(udpi==uDpi)
	return;
uDpi=udpi;
Update();
}

VOID SwapChain::SetSize(UINT uwidth, UINT uheight)
{
if(uwidth==cPanelSize.Width&&uheight==cPanelSize.Height)
	return;
cPanelSize.Width=uwidth;
cPanelSize.Height=uheight;
Update();
}


//==========================
// Con-/Destructors Private
//==========================

VOID SwapChain::Destroy()
{
if(hDeviceContext)
	{
	hDeviceContext->DeviceLost.Remove(this);
	hDeviceContext=nullptr;
	}
pDepthStencilView=nullptr;
pRenderTargetView=nullptr;
pSwapChain=nullptr;
pTargetBitmap=nullptr;
}


//================
// Common Private
//================

VOID SwapChain::Initialize()
{
hDeviceContext->Destroyed.Add(this, &SwapChain::OnDeviceContextDestroyed);
hDeviceContext->DeviceLost.Add(this, &SwapChain::OnDeviceLost);
ZeroMemory(&cDescription, sizeof(DXGI_SWAP_CHAIN_DESC));
cDescription.AlphaMode=DXGI_ALPHA_MODE_IGNORE;
cDescription.BufferCount=2;
cDescription.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
cDescription.Format=SWAPCHAIN_FORMAT;
cDescription.Height=1;
cDescription.SampleDesc.Count=1;
cDescription.Scaling=DXGI_SCALING_STRETCH;
cDescription.SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
cDescription.Width=1;
#ifndef __cplusplus_winrt
if(hWindow)
	{
	RECT rc;
	GetClientRect(hWindow, &rc);
	cPanelSize.Width=rc.right;
	cPanelSize.Height=rc.bottom;
	}
#endif
ZeroMemory(&cViewport, sizeof(D3D_VIEWPORT));
cViewport.Height=1.f;
cViewport.MaxDepth=1.f;
cViewport.Width=1.f;
}

VOID SwapChain::OnDeviceLost(Handle<DeviceContext> hdc)
{
pDepthStencilView=nullptr;
pRenderTargetView=nullptr;
pSwapChain=nullptr;
pTargetBitmap=nullptr;
}

VOID SwapChain::SetRotationInternal(DXGI_MODE_ROTATION urot)
{
if(urot==uRotation)
	return;
uRotation=urot;
switch(uRotation)
	{
	case DXGI_MODE_ROTATION_ROTATE90:
		{
		mxOrientation=mxRotation90;
		break;
		}
	case DXGI_MODE_ROTATION_ROTATE180:
		{
		mxOrientation=mxRotation180;
		break;
		}
	case DXGI_MODE_ROTATION_ROTATE270:
		{
		mxOrientation=mxRotation270;
		break;
		}
	default:
		{
		mxOrientation=mxRotation0;
		break;
		}
	}
}

VOID SwapChain::Update()
{
UINT uwidth=cPanelSize.Width;
UINT uheight=cPanelSize.Height;
if(uRotation==DXGI_MODE_ROTATION_ROTATE90||uRotation==DXGI_MODE_ROTATION_ROTATE270)
	{
	uwidth=cPanelSize.Height;
	uheight=cPanelSize.Width;
	}
uwidth=max(uwidth*uDpi/96, 1);
uheight=max(uheight*uDpi/96, 1);
cDescription.Width=uwidth;
cDescription.Height=uheight;
cViewport.Width=(FLOAT)uwidth;
cViewport.Height=(FLOAT)uheight;
ScopedLock lock(hDeviceContext->GetCriticalSection());
auto pdev=hDeviceContext->GetD3DDevice();
if(pSwapChain)
	{
	hDeviceContext->SetTarget(nullptr);
	pRenderTargetView=nullptr;
	pDepthStencilView=nullptr;
	pTargetBitmap=nullptr;
	HRESULT hr=pSwapChain->ResizeBuffers(2, uwidth, uheight, SWAPCHAIN_FORMAT, 0);
	if(FAILED(hr))
		{
		pSwapChain=nullptr;
		return;
		}
	}
else
	{
	auto pfactory=hDeviceContext->GetDXGIFactory();
	#ifndef __cplusplus_winrt
	if(hWindow)
		{
		Microsoft::WRL::ComPtr<IDXGISwapChain1> psc;
		ThrowIfFailed(pfactory->CreateSwapChainForHwnd(pdev, hWindow, &cDescription, 0, 0, &psc));
		Microsoft::WRL::ComPtr<DXGI_SWAPCHAIN> pswapchain;
		ThrowIfFailed(psc.As(&pswapchain));
		pSwapChain=pswapchain.Get();
		}
	#else
	Microsoft::WRL::ComPtr<IDXGISwapChain1> psc;
	ThrowIfFailed(pfactory->CreateSwapChainForComposition(pdev, &cDescription, 0, &psc));
	Microsoft::WRL::ComPtr<DXGI_SWAPCHAIN> pswapchain;
	ThrowIfFailed(psc.As(&pswapchain));
	pSwapChain=pswapchain.Get();
	Microsoft::WRL::ComPtr<ISwapChainPanelNative> pscpn;
	ThrowIfFailed(pPanel->QueryInterface(IID_PPV_ARGS(&pscpn)));
	ThrowIfFailed(pscpn->SetSwapChain(pSwapChain.Get()));
	#endif
	ThrowIfFailed(hDeviceContext->GetDXGIDevice()->SetMaximumFrameLatency(1));
	}
ThrowIfFailed(pSwapChain->SetRotation(uRotation));
Microsoft::WRL::ComPtr<D3D_TEXTURE> pbackbuf;
ThrowIfFailed(pSwapChain->GetBuffer(0, __uuidof(D3D_TEXTURE), &pbackbuf));
Microsoft::WRL::ComPtr<D3D_RENDER_TARGET_VIEW> prtv;
ThrowIfFailed(pdev->CreateRenderTargetView(pbackbuf.Get(), 0, &prtv));
pRenderTargetView=prtv.Get();
CD3D11_TEXTURE2D_DESC dsd(DXGI_FORMAT_D24_UNORM_S8_UINT, cDescription.Width, cDescription.Height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
Microsoft::WRL::ComPtr<D3D_TEXTURE> pdepthstencil;
ThrowIfFailed(pdev->CreateTexture2D(&dsd, 0, &pdepthstencil));
CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
Microsoft::WRL::ComPtr<D3D_DEPTH_STENCIL_VIEW> pdsv;
ThrowIfFailed(pdev->CreateDepthStencilView(pdepthstencil.Get(), &dsvd, &pdsv));
pDepthStencilView=pdsv.Get();
auto pdc2d=hDeviceContext->GetD2DDeviceContext();
Microsoft::WRL::ComPtr<DXGI_SURFACE> psurface;
ThrowIfFailed(pSwapChain->GetBuffer(0, __uuidof(DXGI_SURFACE), &psurface));
D2D_BITMAP_PROPERTIES bmpp;
ZeroMemory(&bmpp, sizeof(D2D_BITMAP_PROPERTIES));
bmpp.bitmapOptions=D2D1_BITMAP_OPTIONS_CANNOT_DRAW|D2D1_BITMAP_OPTIONS_TARGET;
bmpp.pixelFormat=D2D1::PixelFormat(SWAPCHAIN_FORMAT, D2D1_ALPHA_MODE_PREMULTIPLIED);
bmpp.dpiX=(FLOAT)uDpi;
bmpp.dpiY=(FLOAT)uDpi;
Microsoft::WRL::ComPtr<D2D_BITMAP> ptbmp;
ThrowIfFailed(pdc2d->CreateBitmapFromDxgiSurface(psurface.Get(), &bmpp, &ptbmp));
pTargetBitmap=ptbmp.Get();
pdc2d->SetDpi((FLOAT)uDpi, (FLOAT)uDpi);
}

}}