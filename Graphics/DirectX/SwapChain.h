//=============
// SwapChain.h
//=============

#pragma once


//=======
// Using
//=======

#include "Graphics/Size.h"
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


//============
// Swap-Chain
//============

class SwapChain: public Object
{
public:
	// Con-/Destructors
	#ifndef __cplusplus_winrt
	SwapChain(Handle<DeviceContext> DC, HWND Window, UINT Dpi);
	#else
	SwapChain(Handle<DeviceContext> DC, Windows::UI::Xaml::Controls::SwapChainPanel^ Panel, UINT Dpi, DXGI_MODE_ROTATION Rotation);
	#endif

	// Common
	D3D_DEPTH_STENCIL_VIEW* GetDepthStencilView() { return pDepthStencilView.Get(); }
	UINT GetDotsPerInch() { return uDpi; }
	UINT GetHeight() { return cDescription.Height; }
	D2D1::Matrix3x2F GetOrientationMatrix2D();
	MATRIX const& GetOrientationMatrix3D() { return mxOrientation; }
	D3D_RENDER_TARGET_VIEW* GetRenderTargetView() { return pRenderTargetView.Get(); }
	DXGI_MODE_ROTATION GetRotation() { return uRotation; }
	D2D_BITMAP* GetTargetBitmap() { return pTargetBitmap.Get(); }
	D3D_VIEWPORT* GetViewport() { return &cViewport; }
	UINT GetWidth() { return cDescription.Width; }
	VOID Present();
	VOID Set(UINT Width, UINT Height, UINT Dpi, DXGI_MODE_ROTATION Rotation);
	VOID SetDotsPerInch(UINT Dpi);
	VOID SetRotation(DXGI_MODE_ROTATION Rotation);
	VOID SetSize(UINT Width, UINT Height);

private:
	// Con-/Destructors
	VOID Destroy();

	// Common
	VOID Initialize();
	VOID OnDeviceContextDestroyed(DeviceContext* DeviceContext) { Destroy(); }
	VOID OnDeviceLost(Handle<DeviceContext> DeviceContext);
	VOID SetRotationInternal(DXGI_MODE_ROTATION Rotation);
	VOID Update();
	DXGI_SWAPCHAIN_DESC cDescription;
	SIZE cPanelSize;
	D3D_VIEWPORT cViewport;
	Handle<DeviceContext> hDeviceContext;
	#ifndef __cplusplus_winrt
	HWND hWindow;
	#endif
	MATRIX& mxOrientation;
	ComPtr<D3D_DEPTH_STENCIL_VIEW> pDepthStencilView;
	ComPtr<D3D_RENDER_TARGET_VIEW> pRenderTargetView;
	ComPtr<DXGI_SWAPCHAIN> pSwapChain;
	ComPtr<D2D_BITMAP> pTargetBitmap;
	UINT uDpi;
	DXGI_MODE_ROTATION uRotation;
	#ifdef __cplusplus_winrt
	IUnknown* pPanel;
	#endif
};

}}
