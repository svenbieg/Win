//===========
// Version.h
//===========

#pragma once


//=======
// Using
//=======

#pragma comment(lib, "D2D1")
#pragma comment(lib, "D3D11")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "DXGI")
#pragma comment(lib, "DXGUID")

#include <d2d1.h>
#include <d2d1_2.h>
#include <d3d11_3.h>
#include <DirectXMath.h>
#include <dwrite_2.h>
#include <wincodec.h>


//=======
// Types
//=======

using FLOAT2=DirectX::XMFLOAT2;
using FLOAT3=DirectX::XMFLOAT3;
using FLOAT4=DirectX::XMFLOAT4;
using FLOAT4X4=DirectX::XMFLOAT4X4;
using MATRIX=DirectX::XMMATRIX;
using VECTOR=DirectX::XMVECTOR;


//==========
// Direct2D
//==========

using D2D_BITMAP=ID2D1Bitmap1;
using D2D_BITMAP_PROPERTIES=D2D1_BITMAP_PROPERTIES1;
using D2D_COLOR=D2D1_COLOR_F;
using D2D_DEVICE=ID2D1Device1;
using D2D_DEVICE_CONTEXT=ID2D1DeviceContext1;
using D2D_FACTORY=ID2D1Factory2;
using D2D_IMAGE=ID2D1Image;
using D2D_LAYER=ID2D1Layer;
using D2D_LAYER_PARAMETERS=D2D1_LAYER_PARAMETERS1;
using D2D_MAPPED_RECT=D2D1_MAPPED_RECT;
using D2D_MATRIX=D2D1_MATRIX_3X2_F;
using D2D_POINT=D2D1_POINT_2F;
using D2D_RECT=D2D1_RECT_F;
using D2D_SOLID_COLOR_BRUSH=ID2D1SolidColorBrush;

inline D2D_COLOR const& D2DColor(FLOAT4 const& Color) { return (D2D_COLOR const&)Color; }
inline D2D_RECT D2DRect(RECT const& Rect) { return D2D_RECT({ (FLOAT)Rect.left, (FLOAT)Rect.top, (FLOAT)Rect.right, (FLOAT)Rect.bottom }); }


//==========
// Direct3D
//==========

#define SWAPCHAIN_FORMAT DXGI_FORMAT_B8G8R8A8_UNORM

using D3D_BIND_FLAG=D3D11_BIND_FLAG;
using D3D_BLEND_DESC=D3D11_BLEND_DESC;
using D3D_BLEND_STATE=ID3D11BlendState;
using D3D_BUFFER=ID3D11Buffer;
using D3D_BUFFER_DESC=CD3D11_BUFFER_DESC;
using D3D_DEPTH_STENCIL_DESC=D3D11_DEPTH_STENCIL_DESC;
using D3D_DEPTH_STENCIL_STATE=ID3D11DepthStencilState;
using D3D_DEPTH_STENCIL_VIEW=ID3D11DepthStencilView;
using D3D_DEVICE=ID3D11Device2;
using D3D_DEVICE_CONTEXT=ID3D11DeviceContext3;
using D3D_INFO_QUEUE=ID3D11InfoQueue;
using D3D_INPUT_ELEMENT_DESC=D3D11_INPUT_ELEMENT_DESC;
using D3D_INPUT_LAYOUT=ID3D11InputLayout;
using D3D_MAPPED_SUBRESOURCE=D3D11_MAPPED_SUBRESOURCE;
using D3D_PIXEL_SHADER=ID3D11PixelShader;
using D3D_RASTERIZER_DESC=D3D11_RASTERIZER_DESC;
using D3D_RASTERIZER_STATE=ID3D11RasterizerState;
using D3D_RENDER_TARGET_VIEW=ID3D11RenderTargetView;
using D3D_RENDER_TARGET_VIEW_DESC=D3D11_RENDER_TARGET_VIEW_DESC;
using D3D_SAMPLER_DESC=D3D11_SAMPLER_DESC;
using D3D_SAMPLER_STATE=ID3D11SamplerState;
using D3D_SHADER_RESOURCE_VIEW=ID3D11ShaderResourceView;
using D3D_SUBRESOURCE_DATA=D3D11_SUBRESOURCE_DATA;
using D3D_TEXTURE=ID3D11Texture2D;
using D3D_TEXTURE_DESC=D3D11_TEXTURE2D_DESC;
using D3D_VERTEX_SHADER=ID3D11VertexShader;
using D3D_VIEWPORT=CD3D11_VIEWPORT;

typedef struct
{
FLOAT3 Position;
FLOAT4 Color;
FLOAT2 UV;
}VERTEX;


//=============
// DirectWrite
//=============

using DWRITE_FACTORY=IDWriteFactory2;
using DWRITE_TEXT_FORMAT=IDWriteTextFormat;
using DWRITE_TEXT_LAYOUT=IDWriteTextLayout;
using DXGI_ADAPTER=IDXGIAdapter;
using DXGI_DEVICE=IDXGIDevice3;
using DXGI_FACTORY=IDXGIFactory2;
using DXGI_MATRIX_3X2=DXGI_MATRIX_3X2_F;
using DXGI_OUTPUT=IDXGIOutput1;
using DXGI_SURFACE=IDXGISurface2;
using DXGI_SWAPCHAIN=IDXGISwapChain2;
using DXGI_SWAPCHAIN_DESC=DXGI_SWAP_CHAIN_DESC1;


//=========
// Imaging
//=========

using WIC_BITMAP=IWICBitmap;
using WIC_BITMAPSOURCE=IWICBitmapSource;
using WIC_FORMAT_CONVERTER=IWICFormatConverter;
using WIC_FACTORY=IWICImagingFactory2;
using WIC_STREAM=IWICStream;


//=====
// ARM
//=====

#if(defined(_M_ARM)||defined(_M_ARM64))
#define M128_F32 n128_f32
#else
#define M128_F32 m128_f32
#endif
