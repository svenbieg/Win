//===================
// DeviceContext.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "DeviceContext.h"

using namespace D2D1;
using namespace DirectX;


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//==================
// Con-/Destructors
//==================

DeviceContext::DeviceContext(LUID uid):
// Device
uId(uid),
// Shader
pConstantBuffer(nullptr)
{
CreateApplicationResources();
CreateDeviceResources();
}

DeviceContext::~DeviceContext()
{
Close();
Destroyed(this);
}


//========
// Device
//========

VOID DeviceContext::Clear(FLOAT4 const& vclr)
{
if(!pRenderTargetView)
	return;
pD3DDeviceContext->ClearRenderTargetView(pRenderTargetView.Get(), (FLOAT*)&vclr);
pD3DDeviceContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

VOID DeviceContext::Close()
{
// Application
pD2DFactory=nullptr;
pDWriteFactory=nullptr;
pDXGIFactory=nullptr;
// Device
if(pD3DDeviceContext.Get()!=nullptr)
	{
	D3D_RENDER_TARGET_VIEW* prtv=nullptr;
	pD3DDeviceContext->OMSetRenderTargets(1, &prtv, nullptr);
	pD2DDeviceContext->SetTarget(nullptr);
	}
hSwapChain=nullptr;
pD2DDevice=nullptr;
pD2DDeviceContext=nullptr;
pD3DDevice=nullptr;
pD3DDeviceContext=nullptr;
pDepthStencilView=nullptr;
pDXGIAdapter=nullptr;
pDXGIDevice=nullptr;
pDXGIOutput=nullptr;
pRenderTargetView=nullptr;
// Shader
if(pConstantBuffer)
	{
	AlignedFree(pConstantBuffer);
	pConstantBuffer=nullptr;
	}
pBlendState=nullptr;
pD3DConstantBuffer=nullptr;
pDefaultTexture=nullptr;
pDefaultTextureView=nullptr;
pDefaultSpecularMap=nullptr;
pDefaultSpecularMapView=nullptr;
pDepthStencilState=nullptr;
pInputLayout=nullptr;
pPixelShader=nullptr;
pRasterizerState=nullptr;
pSamplerState=nullptr;
pVertexShader=nullptr;
}

VOID DeviceContext::SetTarget(D3D_RENDER_TARGET_VIEW* prtv, D3D_DEPTH_STENCIL_VIEW* pdsv, D3D_VIEWPORT* pvp)
{
if(!pD3DDevice)
	return;
pRenderTargetView=prtv;
pDepthStencilView=pdsv;
pD3DDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
pD2DDeviceContext->SetTarget(nullptr);
pD3DDeviceContext->RSSetViewports(1, pvp);
}

VOID DeviceContext::SetTarget(Handle<SwapChain> hswapchain)
{
hSwapChain=hswapchain;
pRenderTargetView=hswapchain? hswapchain->GetRenderTargetView(): nullptr;
pDepthStencilView=hswapchain? hswapchain->GetDepthStencilView(): nullptr;
pD3DDeviceContext->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
D2D_BITMAP* pbmp=hswapchain? hswapchain->GetTargetBitmap(): nullptr;
pD2DDeviceContext->SetTarget(pbmp);
D3D_VIEWPORT* pvp=hswapchain? hswapchain->GetViewport(): nullptr;
if(pvp)
	pD3DDeviceContext->RSSetViewports(1, pvp);
if(hSwapChain)
	{
	pD2DDeviceContext->SetTransform(hSwapChain->GetOrientationMatrix2D());
	}
else
	{
	pD2DDeviceContext->SetTransform(Matrix3x2F::Identity());
	}
}


//=========
// Shaders
//=========

VOID DeviceContext::FlushConstantBuffer()
{
pD3DDeviceContext->UpdateSubresource(pD3DConstantBuffer.Get(), 0, nullptr, pConstantBuffer, 0, 0);
}

VOID DeviceContext::SetMaterial(Handle<Material> hmat)
{
D3D_SHADER_RESOURCE_VIEW* ppsrv[2];
ppsrv[0]=pDefaultTextureView.Get();
ppsrv[1]=pDefaultSpecularMapView.Get();
if(hmat)
	{
	if(hmat->Texture)
		ppsrv[0]=hmat->Texture->GetView(this);
	if(hmat->SpecularMap)
		ppsrv[1]=hmat->SpecularMap->GetView(this);
	}
pD3DDeviceContext->PSSetShaderResources(0, 2, ppsrv);
}


//=====================
// Application Private
//=====================

VOID DeviceContext::CreateApplicationResources()
{
D2D1_FACTORY_OPTIONS d2dfo;
ZeroMemory(&d2dfo, sizeof(D2D1_FACTORY_OPTIONS));
#ifdef _DEBUG
d2dfo.debugLevel=D2D1_DEBUG_LEVEL_INFORMATION;
#endif
Microsoft::WRL::ComPtr<D2D_FACTORY> pd2df;
ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(D2D_FACTORY), &d2dfo, &pd2df));
pD2DFactory=pd2df.Get();
Microsoft::WRL::ComPtr<DWRITE_FACTORY> pdwf;
ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(DWRITE_FACTORY), &pdwf));
pDWriteFactory=pdwf.Get();
Microsoft::WRL::ComPtr<DXGI_FACTORY> pdxgif;
ThrowIfFailed(CreateDXGIFactory1(__uuidof(DXGI_FACTORY), &pdxgif));
pDXGIFactory=pdxgif.Get();
}


//================
// Device Private
//================

VOID DeviceContext::CreateD2DResources()
{
Microsoft::WRL::ComPtr<D2D_DEVICE> pd2ddev;
ThrowIfFailed(pD2DFactory->CreateDevice(pDXGIDevice.Get(), &pd2ddev));
pD2DDevice=pd2ddev.Get();
Microsoft::WRL::ComPtr<D2D_DEVICE_CONTEXT> pd2ddc;
ThrowIfFailed(pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pd2ddc));
pD2DDeviceContext=pd2ddc.Get();
pD2DDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

VOID DeviceContext::CreateD3DResources()
{
Microsoft::WRL::ComPtr<IDXGIAdapter1> padapter;
for(UINT u=0; SUCCEEDED(pDXGIFactory->EnumAdapters1(u, &padapter)); u++)
	{
	DXGI_ADAPTER_DESC dsc;
	ThrowIfFailed(padapter->GetDesc(&dsc));
	if(uId.LowPart||uId.HighPart)
		{
		if(memcmp(&dsc.AdapterLuid, &uId, sizeof(LUID)))
			{
			padapter=nullptr;
			continue;
			}
		}
	Microsoft::WRL::ComPtr<DXGI_ADAPTER> pdxgia;
	padapter.As(&pdxgia);
	pDXGIAdapter=pdxgia.Get();
	Microsoft::WRL::ComPtr<ID3D11Device> pdev;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pdc;
	UINT ud3dcf=D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	#if(defined(_DEBUG)&&!defined(_M_ARM))
	ud3dcf|=D3D11_CREATE_DEVICE_DEBUG;
	#endif
	ThrowIfFailed(D3D11CreateDevice(pDXGIAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, ud3dcf, 0, 0, D3D11_SDK_VERSION, &pdev, 0, &pdc));
	Microsoft::WRL::ComPtr<D3D_DEVICE> pd3ddev;
	ThrowIfFailed(pdev.As(&pd3ddev));
	pD3DDevice=pd3ddev.Get();
	Microsoft::WRL::ComPtr<DXGI_DEVICE> pdxgidev;
	ThrowIfFailed(pdev.As(&pdxgidev));
	pDXGIDevice=pdxgidev.Get();
	Microsoft::WRL::ComPtr<D3D_DEVICE_CONTEXT> pd3ddc;
	ThrowIfFailed(pdc.As(&pd3ddc));
	pD3DDeviceContext=pd3ddc.Get();
	ThrowIfFailed(pDXGIDevice->SetMaximumFrameLatency(1));
	Microsoft::WRL::ComPtr<IDXGIOutput> poutput;
	ThrowIfFailed(pDXGIAdapter->EnumOutputs(0, &poutput));
	Microsoft::WRL::ComPtr<DXGI_OUTPUT> pdxgiout;
	ThrowIfFailed(poutput.As(&pdxgiout));
	pDXGIOutput=pdxgiout.Get();
	return;
	}
throw EFAULT;
}

VOID DeviceContext::CreateDeviceResources()
{
CreateD3DResources();
CreateShaderResources();
CreateD2DResources();
}

VOID DeviceContext::HandleDeviceLost()
{
DeviceLost(this);
CreateDeviceResources();
DeviceRestored(this);
}


//=================
// Shaders Private
//=================

VOID DeviceContext::CreateConstantBuffer()
{
pConstantBuffer=(CONSTANT_BUFFER*)AlignedAlloc(sizeof(CONSTANT_BUFFER), 16);
pConstantBuffer->ModelMatrix=XMMatrixIdentity();
pConstantBuffer->ProjectionMatrix=XMMatrixIdentity();
pConstantBuffer->ViewMatrix=XMMatrixIdentity();
pConstantBuffer->AmbientColor={ 1.f, 1.f, 1.f, 1.f };
pConstantBuffer->Color={ 0.f, 0.f, 0.f, 0.f };
D3D_BUFFER_DESC cbdsc(sizeof(CONSTANT_BUFFER), D3D11_BIND_CONSTANT_BUFFER);
Microsoft::WRL::ComPtr<D3D_BUFFER> pcb;
ThrowIfFailed(pD3DDevice->CreateBuffer(&cbdsc, nullptr, &pcb));
pD3DConstantBuffer=pcb.Get();
D3D_BUFFER* pcbufs=pD3DConstantBuffer.Get();
pD3DDeviceContext->VSSetConstantBuffers(0, 1, &pcbufs);
}

VOID DeviceContext::CreateDefaultTextures()
{
D3D_TEXTURE_DESC dsc;
ZeroMemory(&dsc, sizeof(D3D_TEXTURE_DESC));
dsc.Width=1;
dsc.Height=1;
dsc.MipLevels=1;
dsc.ArraySize=1;
dsc.Format=SWAPCHAIN_FORMAT;
dsc.SampleDesc.Count=1;
dsc.Usage=D3D11_USAGE_DEFAULT;
dsc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
Microsoft::WRL::ComPtr<D3D_TEXTURE> ptex;
ThrowIfFailed(pD3DDevice->CreateTexture2D(&dsc, 0, &ptex));
pDefaultTexture=ptex.Get();
Microsoft::WRL::ComPtr<D3D_TEXTURE> pspec;
ThrowIfFailed(pD3DDevice->CreateTexture2D(&dsc, 0, &pspec));
pDefaultSpecularMap=pspec.Get();
Microsoft::WRL::ComPtr<D3D_SHADER_RESOURCE_VIEW> ptexview;
ThrowIfFailed(pD3DDevice->CreateShaderResourceView(pDefaultTexture.Get(), 0, &ptexview));
pDefaultTextureView=ptexview.Get();
Microsoft::WRL::ComPtr<D3D_SHADER_RESOURCE_VIEW> pspecview;
ThrowIfFailed(pD3DDevice->CreateShaderResourceView(pDefaultSpecularMap.Get(), 0, &pspecview));
pDefaultSpecularMapView=pspecview.Get();
UINT uwhite=0xFFFFFFFF;
pD3DDeviceContext->UpdateSubresource(pDefaultTexture.Get(), 0, nullptr, &uwhite, 4, 4);
UINT ublack=0;
pD3DDeviceContext->UpdateSubresource(pDefaultSpecularMap.Get(), 0, nullptr, &ublack, 4, 4);
}

VOID DeviceContext::CreateShaderResources()
{
#include "Shaders/VertexShader.h"
Microsoft::WRL::ComPtr<D3D_VERTEX_SHADER> pvs;
ThrowIfFailed(pD3DDevice->CreateVertexShader(pvertexshader, ARRAYSIZE(pvertexshader), 0, &pvs));
pVertexShader=pvs.Get();
pD3DDeviceContext->VSSetShader(pVertexShader.Get(), 0, 0);
#include "Shaders/PixelShader.h"
Microsoft::WRL::ComPtr<D3D_PIXEL_SHADER> pps;
ThrowIfFailed(pD3DDevice->CreatePixelShader(ppixelshader, ARRAYSIZE(ppixelshader), 0, &pps));
pPixelShader=pps.Get();
pD3DDeviceContext->PSSetShader(pPixelShader.Get(), 0, 0);
static const D3D11_INPUT_ELEMENT_DESC vsidesc[]={
	{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
Microsoft::WRL::ComPtr<D3D_INPUT_LAYOUT> pil;
ThrowIfFailed(pD3DDevice->CreateInputLayout(vsidesc, ARRAYSIZE(vsidesc), pvertexshader, ARRAYSIZE(pvertexshader), &pil));
pInputLayout=pil.Get();
pD3DDeviceContext->IASetInputLayout(pInputLayout.Get());
CreateConstantBuffer();
D3D_DEPTH_STENCIL_DESC dsd;
ZeroMemory(&dsd, sizeof(D3D_DEPTH_STENCIL_DESC));
dsd.DepthEnable=true;
dsd.DepthFunc=D3D11_COMPARISON_ALWAYS;
dsd.DepthWriteMask=D3D11_DEPTH_WRITE_MASK_ALL;
Microsoft::WRL::ComPtr<D3D_DEPTH_STENCIL_STATE> pdss;
ThrowIfFailed(pD3DDevice->CreateDepthStencilState(&dsd, &pdss));
pDepthStencilState=pdss.Get();
pD3DDeviceContext->OMSetDepthStencilState(pDepthStencilState.Get(), 0);
D3D_RASTERIZER_DESC rd;
ZeroMemory(&rd, sizeof(D3D_RASTERIZER_DESC));
rd.AntialiasedLineEnable=rd.DepthClipEnable=true;
rd.CullMode=D3D11_CULL_NONE;
rd.FillMode=D3D11_FILL_SOLID;
Microsoft::WRL::ComPtr<D3D_RASTERIZER_STATE> prrs;
ThrowIfFailed(pD3DDevice->CreateRasterizerState(&rd, &prrs));
pRasterizerState=prrs.Get();
pD3DDeviceContext->RSSetState(pRasterizerState.Get());
D3D_BLEND_DESC bd;
ZeroMemory(&bd, sizeof(D3D_BLEND_DESC));
bd.RenderTarget[0].BlendEnable=true;
bd.RenderTarget[0].BlendOp=bd.RenderTarget[0].BlendOpAlpha=D3D11_BLEND_OP_ADD;
bd.RenderTarget[0].SrcBlend=bd.RenderTarget[0].SrcBlendAlpha=D3D11_BLEND_SRC_ALPHA;
bd.RenderTarget[0].DestBlend=bd.RenderTarget[0].DestBlendAlpha=D3D11_BLEND_INV_SRC_ALPHA;
bd.RenderTarget[0].RenderTargetWriteMask=D3D11_COLOR_WRITE_ENABLE_ALL;
Microsoft::WRL::ComPtr<D3D_BLEND_STATE> pbs;
ThrowIfFailed(pD3DDevice->CreateBlendState(&bd, &pbs));
pBlendState=pbs.Get();
pD3DDeviceContext->OMSetBlendState(pBlendState.Get(), 0, 0xFFFFFFFF);
D3D_SAMPLER_DESC sd;
ZeroMemory(&sd, sizeof(D3D_SAMPLER_DESC));
sd.AddressU=sd.AddressV=sd.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;
sd.Filter=D3D11_FILTER_ANISOTROPIC;
sd.MaxAnisotropy=8;
sd.MinLOD=-FLT_MAX;
sd.MaxLOD=FLT_MAX;
sd.ComparisonFunc=D3D11_COMPARISON_NEVER;
Microsoft::WRL::ComPtr<D3D_SAMPLER_STATE> pss;
ThrowIfFailed(pD3DDevice->CreateSamplerState(&sd, &pss));
pSamplerState=pss.Get();
pD3DDeviceContext->PSSetSamplers(0, 1, pSamplerState.GetAddressOf());
CreateDefaultTextures();
}

}}
