//=============
// Gamepad.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#pragma comment(lib, "Xinput.lib")

#include "Gamepad.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Input {


//==================
// Con-/Destructors
//==================

Gamepad::Gamepad(UINT uid):
bConnected(false),
uId(uid)
{
ZeroMemory(&cGamepad, sizeof(XINPUT_GAMEPAD));
}

Gamepad::~Gamepad()
{
if(hTimer!=nullptr)
	{
	hTimer->Stop();
	hTimer=nullptr;
	}
}


//========
// Common
//========

VOID Gamepad::SetEnabled(BOOL benabled)
{
if(!benabled)
	{
	if(hTimer!=nullptr)
		{
		hTimer->Stop();
		hTimer=nullptr;
		bConnected=false;
		ZeroMemory(&cGamepad, sizeof(XINPUT_GAMEPAD));
		}
	return;
	}
if(hTimer!=nullptr)
	return;
hTimer=new Timer();
hTimer->Triggered.Add(this, &Gamepad::OnTimerTriggered);
hTimer->StartPeriodic(500);
}

BOOL Gamepad::IsButtonDown(GamepadButton btn)
{
switch(btn)
	{
	case GamepadButton::A:
		return cGamepad.wButtons&XINPUT_GAMEPAD_A;
	case GamepadButton::B:
		return cGamepad.wButtons&XINPUT_GAMEPAD_B;
	case GamepadButton::Down:
		return cGamepad.wButtons&XINPUT_GAMEPAD_DPAD_DOWN;
	case GamepadButton::Left:
		return cGamepad.wButtons&XINPUT_GAMEPAD_DPAD_LEFT;
	case GamepadButton::Right:
		return cGamepad.wButtons&XINPUT_GAMEPAD_DPAD_RIGHT;
	case GamepadButton::Start:
		return cGamepad.wButtons&XINPUT_GAMEPAD_START;
	case GamepadButton::StickLeft:
		return cGamepad.wButtons&XINPUT_GAMEPAD_LEFT_THUMB;
	case GamepadButton::StickRight:
		return cGamepad.wButtons&XINPUT_GAMEPAD_RIGHT_THUMB;
	case GamepadButton::Stop:
		return cGamepad.wButtons&XINPUT_GAMEPAD_BACK;
	case GamepadButton::TriggerLeft:
		return cGamepad.wButtons&XINPUT_GAMEPAD_LEFT_SHOULDER;
	case GamepadButton::TriggerRight:
		return cGamepad.wButtons&XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case GamepadButton::Up:
		return cGamepad.wButtons&XINPUT_GAMEPAD_DPAD_UP;
	case GamepadButton::X:
		return cGamepad.wButtons&XINPUT_GAMEPAD_X;
	case GamepadButton::Y:
		return cGamepad.wButtons&XINPUT_GAMEPAD_Y;
	}
return false;
}

BOOL Gamepad::IsEnabled()
{
return hTimer!=nullptr;
}


//================
// Common Private
//================

VOID Gamepad::OnTimerTriggered(Handle<Timer> htimer)
{
XINPUT_STATE state;
if(XInputGetState(uId, &state))
	{
	bConnected=false;
	Disconnected(this);
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	SetState(state.Gamepad);
	return;
	}
if(state.Gamepad.wButtons==0)
	{
	bConnected=true;
	Connected(this);
	}
if(!bConnected)
	return;
SetState(state.Gamepad);
}

VOID Gamepad::SetState(XINPUT_GAMEPAD const& gamepad)
{
BOOL bup=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
BOOL bup1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_UP, bup1);
if(!bup&&bup1)
	ButtonDown(this, GamepadButton::Up);
if(bup&&!bup1)
	ButtonUp(this, GamepadButton::Up);
BOOL bdown=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
BOOL bdown1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_DOWN, bdown1);
if(!bdown&&bdown1)
	ButtonDown(this, GamepadButton::Up);
if(bdown&&!bdown1)
	ButtonUp(this, GamepadButton::Up);
BOOL bleft=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
BOOL bleft1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_LEFT, bleft1);
if(!bleft&&bleft1)
	ButtonDown(this, GamepadButton::Up);
if(bleft&&!bleft1)
	ButtonUp(this, GamepadButton::Up);
BOOL bright=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
BOOL bright1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT, bright1);
if(!bright&&bright1)
	ButtonDown(this, GamepadButton::Up);
if(bright&&!bright1)
	ButtonUp(this, GamepadButton::Up);
BOOL bstart=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_START);
BOOL bstart1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_START);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_START, bstart1);
if(!bstart&&bstart1)
	ButtonDown(this, GamepadButton::Up);
if(bstart&&!bstart1)
	ButtonUp(this, GamepadButton::Up);
BOOL bstop=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_BACK);
BOOL bstop1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_BACK);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_BACK, bstop1);
if(!bstop&&bstop1)
	ButtonDown(this, GamepadButton::Up);
if(bstop&&!bstop1)
	ButtonUp(this, GamepadButton::Up);
BOOL bstickl=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
BOOL bstickl1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_LEFT_THUMB, bstickl1);
if(!bstickl&&bstickl1)
	ButtonDown(this, GamepadButton::Up);
if(bstickl&&!bstickl1)
	ButtonUp(this, GamepadButton::Up);
BOOL bstickr=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
BOOL bstickr1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB, bstickr1);
if(!bstickr&&bstickr1)
	ButtonDown(this, GamepadButton::Up);
if(bstickr&&!bstickr1)
	ButtonUp(this, GamepadButton::Up);
BOOL btrigl=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
BOOL btrigl1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER, btrigl1);
if(!btrigl&&btrigl1)
	ButtonDown(this, GamepadButton::Up);
if(btrigl&&!btrigl1)
	ButtonUp(this, GamepadButton::Up);
BOOL btrigr=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
BOOL btrigr1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER, btrigr1);
if(!btrigr&&btrigr1)
	ButtonDown(this, GamepadButton::Up);
if(btrigr&&!btrigr1)
	ButtonUp(this, GamepadButton::Up);
BOOL ba=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_A);
BOOL ba1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_A);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_A, ba1);
if(!ba&&ba1)
	ButtonDown(this, GamepadButton::Up);
if(ba&&!ba1)
	ButtonUp(this, GamepadButton::Up);
BOOL bb=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_B);
BOOL bb1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_B);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_B, bb1);
if(!bb&&bb1)
	ButtonDown(this, GamepadButton::Up);
if(bb&&!bb1)
	ButtonUp(this, GamepadButton::Up);
BOOL bx=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_X);
BOOL bx1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_X);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_X, bx1);
if(!bx&&bx1)
	ButtonDown(this, GamepadButton::Up);
if(bx&&!bx1)
	ButtonUp(this, GamepadButton::Up);
BOOL by=GetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_Y);
BOOL by1=GetFlag(gamepad.wButtons, XINPUT_GAMEPAD_Y);
SetFlag(cGamepad.wButtons, XINPUT_GAMEPAD_Y, by1);
if(!by&&by1)
	ButtonDown(this, GamepadButton::Up);
if(by&&!by1)
	ButtonUp(this, GamepadButton::Up);
if(cGamepad.bLeftTrigger!=gamepad.bLeftTrigger)
	{
	cGamepad.bLeftTrigger=gamepad.bLeftTrigger;
	LeftTriggerMoved(this, cGamepad.bLeftTrigger*100/255);
	}
if(cGamepad.bRightTrigger!=gamepad.bRightTrigger)
	{
	cGamepad.bRightTrigger=gamepad.bRightTrigger;
	RightTriggerMoved(this, cGamepad.bRightTrigger*100/255);
	}
if(cGamepad.sThumbLX!=gamepad.sThumbLX||cGamepad.sThumbLY!=gamepad.sThumbLY)
	{
	cGamepad.sThumbLX=gamepad.sThumbLX;
	cGamepad.sThumbLY=gamepad.sThumbLY;
	LeftStickMoved(this, cGamepad.sThumbLX*100/32767, cGamepad.sThumbLY*100/32767);
	}
if(cGamepad.sThumbRX!=gamepad.sThumbRX||cGamepad.sThumbRY!=gamepad.sThumbRY)
	{
	cGamepad.sThumbRX=gamepad.sThumbRX;
	cGamepad.sThumbRY=gamepad.sThumbRY;
	RightStickMoved(this, cGamepad.sThumbRX*100/32767, cGamepad.sThumbRY*100/32767);
	}
}

}}
