//=========
// Brush.h
//=========

#pragma once


//=======
// Using
//=======

#include "Collections/Map.h"
#include "Graphics/Color.h"
#include "ComPtr.h"
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


//=======
// Brush
//=======

class Brush: public Object
{
private:
	// Using
	using DeviceMap=Collections::Map<DeviceContext*, ComPtr<D2D_SOLID_COLOR_BRUSH>>;

public:
	// Con-/Destructors
	Brush(COLOR Color);
	Brush(FLOAT Red, FLOAT Green, FLOAT Blue, FLOAT Alpha=1.f);
	Brush(FLOAT4 const& Color);
	#ifdef __cplusplus_winrt
	Brush(Windows::UI::Color Color);
	#endif
	~Brush();

	// Common
	FLOAT GetAlpha() { return vColor.w; }
	FLOAT GetBlue() { return vColor.z; }
	FLOAT4 const& GetBuffer() { return vColor; }
	D2D_SOLID_COLOR_BRUSH* GetBrush(Handle<DeviceContext> DeviceContext);
	FLOAT GetGreen() { return vColor.y; }
	FLOAT GetRed() { return vColor.x; }
	COLOR GetValue() { return uColor; }
	#ifdef __cplusplus_winrt
	Windows::UI::Color GetUIColor();
	#endif

private:
	// Common
	VOID OnDeviceContextDestroyed(DeviceContext* DeviceContext);
	VOID OnDeviceLost(Handle<DeviceContext> DeviceContext);
	VOID RemoveDevice(DeviceContext* DeviceContext);
	Handle<DeviceMap> hBrushes;
	COLOR uColor;
	FLOAT4 vColor;
};

}}
