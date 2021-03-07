//============
// Bitmap.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "Bitmap.h"
#include "DeviceContext.h"
#include "ResourceHelper.h"

using namespace D2D1;
using namespace Storage;


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//==================
// Con-/Destructors
//==================

Bitmap::Bitmap(UINT uid, UINT usize)
{
hBitmaps=new DeviceMap();
BITMAPINFO* pinfo=GetResourceIcon(uid, usize);
uHeight=pinfo->bmiHeader.biHeight;
uWidth=pinfo->bmiHeader.biWidth;
uPitch=uWidth*4;
hBuffer=new Buffer(pinfo->bmiColors, uHeight*uPitch, BufferType::Static);
}

Bitmap::Bitmap(UINT uwidth, UINT uheight, BYTE const* pbuf):
uHeight(uheight),
uPitch(uwidth),
uWidth(uwidth)
{
hBitmaps=new DeviceMap();
hBuffer=new Buffer(pbuf, uHeight*uPitch);
}

Bitmap::Bitmap(Handle<DeviceContext> hdc, Handle<String> hpath):
hPath(hpath),
uHeight(0),
uPitch(0),
uWidth(0)
{
hBitmaps=new DeviceMap();
Microsoft::WRL::ComPtr<WIC_FACTORY> pfactory;
ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, 0, CLSCTX_INPROC_SERVER, __uuidof(WIC_FACTORY), &pfactory));
Microsoft::WRL::ComPtr<WIC_STREAM> pstream;
ThrowIfFailed(pfactory->CreateStream(&pstream));
WCHAR ppath[MAX_PATH];
StringCopy(ppath, MAX_PATH, hPath->Begin());
ThrowIfFailed(pstream->InitializeFromFilename(ppath, GENERIC_READ));
OpenStream(hdc, pfactory.Get(), pstream.Get());
}

Bitmap::Bitmap(Handle<DeviceContext> hdc, BYTE const* pbuf, UINT usize):
uHeight(0),
uPitch(0),
uWidth(0)
{
hBitmaps=new DeviceMap();
Microsoft::WRL::ComPtr<WIC_FACTORY> pfactory;
ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory2, 0, CLSCTX_INPROC_SERVER, __uuidof(WIC_FACTORY), &pfactory));
Microsoft::WRL::ComPtr<WIC_STREAM> pstream;
ThrowIfFailed(pfactory->CreateStream(&pstream));
ThrowIfFailed(pstream->InitializeFromMemory((BYTE*)pbuf, usize));
OpenStream(hdc, pfactory.Get(), pstream.Get());
}

Bitmap::~Bitmap()
{
for(auto hit=hBitmaps->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto pdc=hit->GetCurrentId();
	pdc->Destroyed.Remove(this);
	pdc->DeviceLost.Remove(this);
	}
}


//========
// Access
//========

VOID Bitmap::Draw(Handle<DeviceContext> hdc)
{
ComPtr<D2D_BITMAP> pbmp=GetBitmap(hdc);
hdc->GetD2DDeviceContext()->DrawBitmap(pbmp.Get());
}

VOID Bitmap::Draw(Handle<DeviceContext> hdc, FLOAT x, FLOAT y, FLOAT width, FLOAT height)
{
if(width==0||height==0)
	return;
ComPtr<D2D_BITMAP> pbmp=GetBitmap(hdc);
D2D1_RECT_F rcf;
ZeroMemory(&rcf, sizeof(D2D1_RECT_F));
rcf.left=x;
rcf.top=y;
rcf.right=x+width;
rcf.bottom=y+height;
hdc->GetD2DDeviceContext()->DrawBitmap(pbmp.Get(), &rcf);
}

D2D_BITMAP* Bitmap::GetBitmap(Handle<DeviceContext> hdc)
{
ComPtr<D2D_BITMAP> pbmp;
if(hBitmaps->TryGet(hdc, &pbmp))
	{
	if(pbmp.Get())
		return pbmp.Get();
	}
Microsoft::WRL::ComPtr<D2D_BITMAP> pnew;
CreateBitmap(hdc, pnew.GetAddressOf());
pbmp=pnew.Get();
hBitmaps->Set(hdc, pbmp);
hdc->Destroyed.Add(this, &Bitmap::OnDeviceContextDestroyed);
hdc->DeviceLost.Add(this, &Bitmap::OnDeviceLost);
return pbmp.Get();
}

Handle<Bitmap> Bitmap::Scale(Handle<DeviceContext> hdc, UINT uwidth, UINT uheight)
{
ComPtr<D2D_BITMAP> pbmp=GetBitmap(hdc);
auto pdc=hdc->GetD2DDeviceContext();
D2D_BITMAP_PROPERTIES cbmpp;
ZeroMemory(&cbmpp, sizeof(D2D_BITMAP_PROPERTIES));
cbmpp.bitmapOptions=D2D1_BITMAP_OPTIONS_TARGET;
cbmpp.pixelFormat=PixelFormat(SWAPCHAIN_FORMAT, D2D1_ALPHA_MODE_IGNORE);
Microsoft::WRL::ComPtr<D2D_BITMAP> pdst;
ThrowIfFailed(pdc->CreateBitmap(SizeU(uwidth, uheight), nullptr, 0, cbmpp, &pdst));
pdc->BeginDraw();
pdc->SetTarget(pdst.Get());
pdc->DrawBitmap(pbmp.Get(), RectF(0, 0, (FLOAT)uwidth, (FLOAT)uheight), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
pdc->EndDraw();
cbmpp.bitmapOptions=D2D1_BITMAP_OPTIONS_CANNOT_DRAW|D2D1_BITMAP_OPTIONS_CPU_READ;
Microsoft::WRL::ComPtr<D2D_BITMAP> pbmpmem;
ThrowIfFailed(pdc->CreateBitmap(SizeU(uwidth, uheight), nullptr, 0, cbmpp, &pbmpmem));
pbmpmem->CopyFromBitmap(nullptr, pdst.Get(), nullptr);
D2D_MAPPED_RECT cmap;
pbmpmem->Map(D2D1_MAP_OPTIONS_READ, &cmap);
Handle<Bitmap> hbmp=new Bitmap(uwidth, uheight, cmap.bits);
pbmpmem->Unmap();
return hbmp;
}


//================
// Common Private
//================

VOID Bitmap::CreateBitmap(Handle<DeviceContext> hdc, D2D_BITMAP** ppbmp)
{
auto pdc=hdc->GetD2DDeviceContext();
D2D_BITMAP_PROPERTIES bmpp;
ZeroMemory(&bmpp, sizeof(D2D_BITMAP_PROPERTIES));
bmpp.bitmapOptions=D2D1_BITMAP_OPTIONS_NONE;
bmpp.pixelFormat=PixelFormat(SWAPCHAIN_FORMAT, D2D1_ALPHA_MODE_IGNORE);
ThrowIfFailed(pdc->CreateBitmap(SizeU(uWidth, uHeight), hBuffer->Begin(), uPitch, bmpp, ppbmp));
}

VOID Bitmap::OnDeviceContextDestroyed(DeviceContext* pdc)
{
RemoveDevice(pdc);
}

VOID Bitmap::OnDeviceLost(Handle<DeviceContext> hdc)
{
RemoveDevice(hdc);
}

VOID Bitmap::OpenStream(Handle<DeviceContext> hdc, WIC_FACTORY* pfactory, WIC_STREAM* pstream)
{
Microsoft::WRL::ComPtr<IWICBitmapDecoder> pdecoder;
ThrowIfFailed(pfactory->CreateDecoderFromStream(pstream, 0, WICDecodeMetadataCacheOnLoad, &pdecoder));
Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pframe;
ThrowIfFailed(pdecoder->GetFrame(0, &pframe));
Microsoft::WRL::ComPtr<WIC_FORMAT_CONVERTER> pconverter;
ThrowIfFailed(pfactory->CreateFormatConverter(&pconverter));
ThrowIfFailed(pconverter->Initialize(pframe.Get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.l, WICBitmapPaletteTypeMedianCut));
Microsoft::WRL::ComPtr<WIC_BITMAP> pwicbmp;
ThrowIfFailed(pfactory->CreateBitmapFromSource(pconverter.Get(), WICBitmapNoCache, &pwicbmp));
Microsoft::WRL::ComPtr<D2D_BITMAP> pbitmap;
auto pdc=hdc->GetD2DDeviceContext();
D2D_BITMAP_PROPERTIES cbmpp;
ZeroMemory(&cbmpp, sizeof(D2D_BITMAP_PROPERTIES));
cbmpp.bitmapOptions=D2D1_BITMAP_OPTIONS_CANNOT_DRAW|D2D1_BITMAP_OPTIONS_CPU_READ;
cbmpp.pixelFormat=PixelFormat(SWAPCHAIN_FORMAT, D2D1_ALPHA_MODE_IGNORE);
ThrowIfFailed(pdc->CreateBitmapFromWicBitmap(pwicbmp.Get(), &cbmpp, &pbitmap));
D2D1_SIZE_F csize=pbitmap->GetSize();
uWidth=(UINT)csize.width;
uHeight=(UINT)csize.height;
D2D1_MAPPED_RECT map;
pbitmap->Map(D2D1_MAP_OPTIONS_READ, &map);
uPitch=map.pitch;
hBuffer=new Buffer(map.bits, uHeight*uPitch);
pbitmap->Unmap();
}

VOID Bitmap::RemoveDevice(DeviceContext* pdc)
{
auto hit=hBitmaps->Find(pdc);
if(!hit->HasCurrent())
	return;
if(hit->GetCurrentId()!=pdc)
	return;
pdc->Destroyed.Remove(this);
pdc->DeviceLost.Remove(this);
hit->RemoveCurrent();
}

}}
