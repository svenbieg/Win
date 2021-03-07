//===========
// Gamepad.h
//===========

#pragma once


//=======
// Using
//=======

#include <Xinput.h>
#include "Devices/Timer.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Input {


//=========
// Buttons
//=========

enum class GamepadButton
{
A, B, X, Y,
Left, Right, Up, Down,
Start, Stop,
StickLeft, StickRight,
TriggerLeft, TriggerRight
};


//=========
// Gamepad
//=========

class Gamepad: public Object
{
private:
	// Using
	using Timer=::Devices::Timer;

public:
	// Con-/Destructors
	Gamepad(UINT Id=0);
	~Gamepad();

	// Common
	Event<Gamepad, GamepadButton> ButtonDown;
	Event<Gamepad, GamepadButton> ButtonUp;
	Event<Gamepad> Connected;
	Event<Gamepad> Disconnected;
	INT GetLeftStickX()const { return cGamepad.sThumbLX*100/32767; }
	INT GetLeftStickY()const { return cGamepad.sThumbLY*100/32767; }
	INT GetLeftTrigger()const { return cGamepad.bLeftTrigger*100/255; }
	INT GetRightStickX()const { return cGamepad.sThumbRX*100/32767; }
	INT GetRightStickY()const { return cGamepad.sThumbRY*100/32767; }
	INT GetRightTrigger()const { return cGamepad.bRightTrigger*100/255; }
	BOOL IsButtonDown(GamepadButton Button);
	BOOL IsConnected()const { return bConnected; }
	BOOL IsEnabled();
	Event<Gamepad, INT, INT> LeftStickMoved;
	Event<Gamepad, UINT> LeftTriggerMoved;
	Event<Gamepad, INT, INT> RightStickMoved;
	Event<Gamepad, UINT> RightTriggerMoved;
	VOID SetEnabled(BOOL Enabled);

private:
	// Common
	VOID OnTimerTriggered(Handle<Timer> Timer);
	VOID SetState(XINPUT_GAMEPAD const& Gamepad);
	BOOL bConnected;
	XINPUT_GAMEPAD cGamepad;
	Handle<Timer> hTimer;
	UINT uId;
};

}}
