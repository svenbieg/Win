//===========
// Brush.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <wrl.h>
#include "Brush.h"
#include "ColorHelper.h"
#include "DeviceContext.h"


//===========
// Namespace
//===========

namespace Graphics {
	namespace DirectX {


//==================
// Con-/Destructors
//==================

Brush::Brush(COLOR c):
uColor(c)
{
hBrushes=new DeviceMap();
vColor=ColorToVector(uColor);
}

Brush::Brush(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
hBrushes=new DeviceMap();
vColor.x=r;
vColor.y=g;
vColor.z=b;
vColor.w=a;
uColor=ColorToValue(vColor);
}

Brush::Brush(FLOAT4 const& c):
vColor(c)
{
hBrushes=new DeviceMap();
uColor=ColorToValue(vColor);
}

#ifdef __cplusplus_winrt
Brush::Brush(Windows::UI::Color c)
{
hBrushes=new DeviceMap();
uColor=ColorFromUIColor(c);
vColor=ColorToVector(uColor);
}
#endif

Brush::~Brush()
{
for(auto hit=hBrushes->First(); hit->HasCurrent(); hit->MoveNext())
	{
	auto pdc=hit->GetCurrentId();
	pdc->Destroyed.Remove(this);
	pdc->DeviceLost.Remove(this);
	}
}


//========
// Common
//========

D2D_SOLID_COLOR_BRUSH* Brush::GetBrush(Handle<DeviceContext> hdc)
{
ComPtr<D2D_SOLID_COLOR_BRUSH> pbrush;
if(hBrushes->TryGet(hdc, &pbrush))
	{
	if(pbrush.Get())
		return pbrush.Get();
	}
Microsoft::WRL::ComPtr<D2D_SOLID_COLOR_BRUSH> pnew;
hdc->GetD2DDeviceContext()->CreateSolidColorBrush(D2DColor(vColor), pnew.GetAddressOf());
pbrush=pnew.Get();
hBrushes->Set(hdc, pbrush);
return pbrush.Get();
}

#ifdef __cplusplus_winrt
Windows::UI::Color Brush::GetUIColor()
{
return Windows::UI::ColorHelper::FromArgb(uColor>>24, uColor&0xFF, (uColor>>8)&0xFF, (uColor>>16)&0xFF);
}
#endif


//================
// Common Private
//================

VOID Brush::OnDeviceContextDestroyed(DeviceContext* pdc)
{
RemoveDevice(pdc);
}

VOID Brush::OnDeviceLost(Handle<DeviceContext> hdc)
{
RemoveDevice(hdc);
}

VOID Brush::RemoveDevice(DeviceContext* pdc)
{
auto hit=hBrushes->Find(pdc);
if(!hit->HasCurrent())
	return;
if(hit->GetCurrentId()!=pdc)
	return;
pdc->Destroyed.Remove(this);
pdc->DeviceLost.Remove(this);
hit->RemoveCurrent();
}

}}