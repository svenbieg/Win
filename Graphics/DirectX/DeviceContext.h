//=================
// DeviceContext.h
//=================

#pragma once


//=======
// Using
//=======

#include "Graphics/Color.h"
#include "Graphics/Material.h"
#include "Shaders/ConstantBuffer.h"
#include "SwapChain.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//================
// Device-Context
//================

class DeviceContext: public Object
{
private:
	// Using
	using CONSTANT_BUFFER=Shaders::CONSTANT_BUFFER;

public:
	// Con-/Destructors
	DeviceContext(LUID Adapter={ 0, 0 });
	~DeviceContext();
	Event<VOID, DeviceContext*> Destroyed;

	// Application
	D2D_FACTORY* GetD2DFactory() { return pD2DFactory.Get(); }
	DWRITE_FACTORY* GetDWriteFactory() { return pDWriteFactory.Get(); }
	DXGI_FACTORY* GetDXGIFactory() { return pDXGIFactory.Get(); }

	// Device
	VOID BeginDraw() { pD2DDeviceContext->BeginDraw(); }
	VOID Clear(FLOAT4 const& Color);
	VOID Close();
	Event<DeviceContext> DeviceLost;
	Event<DeviceContext> DeviceRestored;
	VOID EndDraw() { pD2DDeviceContext->EndDraw(); }
	CriticalSection& GetCriticalSection() { return cCriticalSection; }
	D2D_DEVICE* GetD2DDevice() { return pD2DDevice.Get(); }
	D2D_DEVICE_CONTEXT* GetD2DDeviceContext() { return pD2DDeviceContext.Get(); }
	D3D_DEVICE* GetD3DDevice() { return pD3DDevice.Get(); }
	D3D_DEVICE_CONTEXT* GetD3DDeviceContext() { return pD3DDeviceContext.Get(); }
	DXGI_ADAPTER* GetDXGIAdapter() { return pDXGIAdapter.Get(); }
	DXGI_DEVICE* GetDXGIDevice() { return pDXGIDevice.Get(); }
	DXGI_OUTPUT* GetDXGIOutput() { return pDXGIOutput.Get(); }
	Handle<SwapChain> GetSwapChain() { return hSwapChain; }
	VOID Lock() { cCriticalSection.Lock(); }
	VOID SetTarget(Handle<SwapChain> SwapChain);
	VOID SetTarget(D3D_RENDER_TARGET_VIEW* RenderTargetView, D3D_DEPTH_STENCIL_VIEW* DepthStencilView, D3D_VIEWPORT* ViewPort);
	VOID Trim() { pDXGIDevice->Trim(); }
	VOID Unlock() { cCriticalSection.Unlock(); }
	VOID WaitForVBlank() { if(pDXGIOutput)pDXGIOutput->WaitForVBlank(); }

	// Shader
	FLOAT4 const& GetAmbientColor() { return pConstantBuffer->AmbientColor; }
	D3D_BLEND_STATE* GetBlendState() { return pBlendState.Get(); }
	FLOAT4 const& GetColor() { return pConstantBuffer->Color; }
	D3D_DEPTH_STENCIL_STATE* GetDepthStencilState() { pDepthStencilState.Get(); }
	VOID FlushConstantBuffer();
	MATRIX const& GetModelMatrix() { return pConstantBuffer->ModelMatrix; }
	MATRIX const& GetProjectionMatrix() { return pConstantBuffer->ProjectionMatrix; }
	D3D_RASTERIZER_STATE* GetRasterizerState() { return pRasterizerState.Get(); }
	D3D_SAMPLER_STATE* GetSamplerState() { return pSamplerState.Get(); }
	MATRIX const& GetViewMatrix() { return pConstantBuffer->ViewMatrix; }
	VOID SetAmbientColor(FLOAT4 const& Color) { pConstantBuffer->AmbientColor=Color; }
	VOID SetColor(FLOAT4 const& Color) { pConstantBuffer->Color=Color; }
	VOID SetMaterial(Handle<Material> Material);
	VOID SetModelMatrix(MATRIX const& Matrix) { pConstantBuffer->ModelMatrix=Matrix; }
	VOID SetProjectionMatrix(MATRIX const& Matrix) { pConstantBuffer->ProjectionMatrix=Matrix; }
	VOID SetViewMatrix(MATRIX const& Matrix) { pConstantBuffer->ViewMatrix=Matrix; }

private:
	// Application
	VOID CreateApplicationResources();
	ComPtr<D2D_FACTORY> pD2DFactory;
	ComPtr<DWRITE_FACTORY> pDWriteFactory;
	ComPtr<DXGI_FACTORY> pDXGIFactory;

	// Device
	VOID CreateD2DResources();
	VOID CreateD3DResources();
	VOID CreateDeviceResources();
	VOID HandleDeviceLost();
	CriticalSection cCriticalSection;
	Handle<SwapChain> hSwapChain;
	ComPtr<D2D_DEVICE> pD2DDevice;
	ComPtr<D2D_DEVICE_CONTEXT> pD2DDeviceContext;
	ComPtr<D3D_DEVICE> pD3DDevice;
	ComPtr<D3D_DEVICE_CONTEXT> pD3DDeviceContext;
	ComPtr<D3D_DEPTH_STENCIL_VIEW> pDepthStencilView;
	ComPtr<DXGI_ADAPTER> pDXGIAdapter;
	ComPtr<DXGI_DEVICE> pDXGIDevice;
	ComPtr<DXGI_OUTPUT> pDXGIOutput;
	ComPtr<D3D_RENDER_TARGET_VIEW> pRenderTargetView;
	LUID uId;

	// Shaders
	VOID CreateConstantBuffer();
	VOID CreateDefaultTextures();
	VOID CreateShaderResources();
	ComPtr<D3D_BLEND_STATE> pBlendState;
	CONSTANT_BUFFER* pConstantBuffer;
	ComPtr<D3D_BUFFER> pD3DConstantBuffer;
	ComPtr<D3D_TEXTURE> pDefaultTexture;
	ComPtr<D3D_SHADER_RESOURCE_VIEW> pDefaultTextureView;
	ComPtr<D3D_TEXTURE> pDefaultSpecularMap;
	ComPtr<D3D_SHADER_RESOURCE_VIEW> pDefaultSpecularMapView;
	ComPtr<D3D_DEPTH_STENCIL_STATE> pDepthStencilState;
	ComPtr<D3D_INPUT_LAYOUT> pInputLayout;
	ComPtr<D3D_PIXEL_SHADER> pPixelShader;
	ComPtr<D3D_RASTERIZER_STATE> pRasterizerState;
	ComPtr<D3D_SAMPLER_STATE> pSamplerState;
	ComPtr<D3D_VERTEX_SHADER> pVertexShader;
};

}}
