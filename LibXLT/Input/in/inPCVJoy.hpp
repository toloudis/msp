/*****************************************************************************
**  inPCVJoy.cpp
**
**      inPCVJoy holds multiple inDevices and queries each to determine
**		the state of a Terawatt button given the state of all inDevices its holding
**		it is derived from inVirtualJoystick to provide a windows specific
**		implementation
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include <map>
#include <vector>

#ifdef IN_PCVJOY_HPP
#error inPCVJoy.hpp multiply included
#endif
#define IN_PCVJOY_HPP

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif
#ifndef IN_KEYBOARD_HPP
#include "Input/in/inKeyboard.hpp"
#endif
#ifndef IN_MOUSE_HPP
#include "Input/in/inMouse.hpp"
#endif
#ifndef IN_GAMEPAD_HPP
#include "Input/in/inGamepad.hpp"
#endif
#ifndef IN_JOYSTICK_HPP
#include "Input/in/inJoystick.hpp"
#endif
#ifndef IN_VIRTUALJOYSTICK_HPP
#include "Input/in/inVirtualJoystick.hpp"
#endif
#ifndef IN_DIGITALDIR_HPP
#include "Input/in/inDigitalDir.hpp"
#endif
#ifndef MA_VECTOR2D_HPP
#include "Core/ma/maVector2d.hpp"
#endif

class inPCVJoy : public inVirtualJoystick
{
public:
	//========================================================================
	//	enums
	//========================================================================
	enum { e_ANYBUTTON = -1 };
	enum DeviceType
	{
		e_KEYBOARD = 0,
		e_MOUSE,
		e_GAMEPAD,
		e_JOYSTICK,
		e_INVALIDDEVICE,
	};

public:
	//========================================================================
	//	Constructor
	//========================================================================
	inPCVJoy(int i_NumSticks = 5);

	//========================================================================
	//	Destructor
	//========================================================================
	~inPCVJoy();

	//========================================================================
	//	IsPressed returns true if any of the devices registered currently have
	//	a button pressed that maps to i_TerawattButton. Can also return a value
	//	from 0 to 1 in o_AnalogPressure representing how far the button is being
	//	pushed. Pass in e_ANYBUTTON to check if any button is pressed, Not a 
	//	valid parameter when requesting pressure info.
	//========================================================================
	bool virtual IsPressed(int i_TerawattButton) const;
	bool virtual IsPressed(int i_TerawattButton, float& o_AnalogPressure) const;

	//========================================================================
	//	IsHeld returns true if any of the devices registered currently have and
	//	also had a button pressed that maps to i_TerawattButton. Can also return
	//	a value from 0 to 1 in o_AnalogPressure representing how far the button
	//	is being pushed. Pass in e_ANYBUTTON to check if any button is pressed, Not a 
	//	valid parameter when requesting pressure info.
	//========================================================================
	bool virtual IsHeld(int i_TerawattButton) const;
	bool virtual IsHeld(int i_TerawattButton, float& o_AnalogPressure) const;

	//========================================================================
	//	IsReleased returns true if any of the devices registered currently have
	//	a button pressed that maps to i_TerawattButton. Pass in e_ANYBUTTON to
	//	check if any button is pressed.
	//========================================================================
	bool virtual IsReleased(int i_TerawattButton) const;

	//========================================================================
	//	IsDirPressed returns 0 if there is no direction being pressed on 
	//	i_TerawattStick, else returns 1
	//========================================================================
	bool virtual IsDirPressed(int i_TerawattStick) const;

	//========================================================================
	//	GetDigitalDir returns an enumeration value representing the direction
	//	the actual joystick is being pushed in.
	//========================================================================
	inDigitalDir::DigitalDir virtual GetDigitalDir(int i_TerawattStick) const;

	//========================================================================
	//	GetAnalogDir returns a float in radians representing the direction the
	//	actual joystick is being pushed in. o_AnalogPressure and o_Twist are
	//	values from 0 to 1 representing how far the stick is being pushed and
	//	far it is being rotated respectively.
	//	The non return value versions give stick location in cartesian style
	//	coordinates.
	//========================================================================
	float virtual GetAnalogDir(int i_TerawattStick) const;
	float virtual GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure) const;
	float virtual GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure, float& o_Twist) const;
	void virtual GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y) const;
	void virtual GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y, int& o_z) const;

	//========================================================================
	//	GetAnalogAxes returns a representation of the position of the joystick
	//	along the x and y axes from -1 to 1, great for when you want the
	//	direction and intensity of the joystick in terms of x and y
	//	North and West (ie Up and Left on a joystick) are negative, South and
	//	East are positive
	//========================================================================
	void virtual GetAnalogAxes(int i_TerawattStick, float& o_x, float& o_y) const;

	//========================================================================
	//	ConsumePressed consumes the terawatt key pressed.  Any subsequent calls
	//	to IsPressed will return false
	//========================================================================
	void ConsumePressed(int i_TerawattButton);

	//========================================================================
	//	ConsumeHeld consumes the terawatt key held.  Any subsequent calls
	//	to IsHeld will return false
	//========================================================================
	void ConsumeHeld(int i_TerawattButton);

	//========================================================================
	//	ConsumeReleased consumes the terawatt key released.  Any subsequent calls
	//	to IsReleased will return false
	//========================================================================
	void ConsumeReleased(int i_TerawattButton);

	//========================================================================
	//	ConsumeDir consumes the terawatt stick dir.  Any subsequent calls
	//	to IsDirPressed will return false
	//========================================================================
	void ConsumeDir(int i_TerawattStick);

	//========================================================================
	//	IsPressedConsumed returns true if the pressed state was consumed this loop
	//========================================================================
	bool IsPressedConsumed(int i_TerawattButton) const;

	//========================================================================
	//	IsHeldConsumed returns true if the held state was consumed this loop
	//========================================================================
	bool IsHeldConsumed(int i_TerawattButton) const;

	//========================================================================
	//	IsReleasedConsumed returns true if the released state was consumed this loop
	//========================================================================
	bool IsReleasedConsumed(int i_TerawattButton) const;

	//========================================================================
	//	IsDirConsumed returns true if the dir state was consumed this loop
	//========================================================================
	bool IsDirConsumed(int i_TerawattStick) const;

	//========================================================================
	//	IsAxisInverted returns whether the given stick on the given device is
	//	currently set to be inverted
	//========================================================================
	bool IsXAxisInverted(inDevice *i_Device, int i_TerawattStick);
	bool IsYAxisInverted(inDevice *i_Device, int i_TerawattStick);
	bool IsZAxisInverted(inDevice *i_Device, int i_TerawattStick);

	//========================================================================
	//	InvertAxis sets the invert parameter for the given stick on the given
	//	device
	//========================================================================
	void InvertXAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert);
	void InvertYAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert);
	void InvertZAxis(inDevice *i_Device, int i_TerawattStick, bool i_Invert);

	//========================================================================
	//	RemoveMapping removes the the given mapping
	//========================================================================
	void RemoveMapping(inDevice *i_Device, int i_TerawattButton, int i_ActualButton);

	//========================================================================
	//	MapButtons maps the given TerawattButton(s) to the given ActualButtons(s)
	//	on the supplied inDevice. The i_Device will be added to the inVirtualJoystick
	//	if it is not already listed there.
	//========================================================================
	void MapButtons(inDevice *i_Device, int i_TerawattButton, int i_ActualButton);
	void MapButtons(inDevice *i_Device, const std::vector<int>& i_TerawattButton, const std::vector<int>& i_ActualButton);

	//========================================================================
	//	MapStick maps the given TerawattStick to the given ActualStick
	//	on the supplied inDevice. The i_Device will be added to the inVirtualJoystick
	//	if it is not already listed there.
	//========================================================================
	void MapStick(inDevice *i_Device, int i_TerawattStick, int i_ActualStick);
	
	//========================================================================
	//	RemoveStick removes the the given TerawattStick, all mappings are thus
	//  removed.
	//========================================================================
	void RemoveStick(inDevice *i_Device, int i_TerawattButton, int i_ActualStick);

	//========================================================================
	//	MapDigitalDir maps the given TerawattStick to the given ActualButtons(s)
	//	on the supplied inDevice. The i_Device will be added to the inVirtualJoystick
	//	if it is not already listed there.
	//========================================================================
	void MapDigitalDir(inDevice *i_Device, int i_TerawattStick, int i_North, int i_East, int i_South, int i_West);

	//========================================================================
	//	SetStickSensitivity sets the sensitivty to use on the given device and stick
	//	this is like in FPS's where you change the mouse sensitivity
	//========================================================================
	void SetStickSensitivity(inDevice *i_Device, int i_TerawattStick, float i_StickSensitivity);
	
	//========================================================================
	//	Think resets all consumed maps
	//========================================================================
	void virtual Think();


private:
	//========================================================================
	//	structs
	//========================================================================
	struct DigitalMap
	{
		int m_North;
		int m_East;
		int m_South;
		int m_West;
	};

	struct DeviceMap
	{
		DeviceMap();
		DeviceType m_Type;
		inKeyboard *m_Keyboard;
		inMouse *m_Mouse;
		inGamepad *m_Gamepad;
		inJoystick *m_Joystick;
		std::multimap<int, int> m_Buttons;
		std::vector<int> m_Sticks;
		std::vector<float> m_StickSensitivity;
		std::vector<DigitalMap> m_DigitalMaps;
		std::vector<bool> m_bInvertX;
		std::vector<bool> m_bInvertY;
		std::vector<bool> m_bInvertZ;
		bool operator==(const DeviceMap& DMap);
		bool IsDeviceEnabled() const;
	};

	//========================================================================
	//	FindActualButton will find all actual buttons mapped to the given 
	//	TerawattButton in the given DeviceMap and return them in the 
	//	o_ActualButton vector
	//========================================================================
	bool FindActualButtons(const DeviceMap& i_Map, int i_TerawattButton, std::vector<int>& o_ActualButton) const;

	//========================================================================
	//	FindDeviceIndex returns the index into the m_Maps vector that holds the
	//	DeviceMap containing i_Device. It will add a new DeviceMap to the m_Maps
	//	vector if i_Device is not yet listed, and then return the index of this
	//	new DeviceMap.
	//	it will also accept a terawattstick as a parameter and then handle 
	//	the task of properly sizing the various stick vectors.
	//========================================================================
	int FindDeviceIndex(inDevice *i_Device);
	int FindDeviceIndex(inDevice *i_Device, int i_TerawattStick);

	//========================================================================
	//	InvertDigitalDir will remap io_Dir by inverting it according to i_bInvertX
	//	and i_bInvertY
	//========================================================================
	void InvertDigitalDir(inDigitalDir::DigitalDir& io_Dir, bool i_bInvertX, bool i_bInvertY) const;

	//========================================================================
	//	InvertAnalogDir will remap the directions by inverting them according to i_bInvertX
	//	and i_bInvertY
	//========================================================================
	void InvertAnalogDir(float& io_Axis, bool i_bInvertAxis) const;
	void InvertAnalogDir(float& io_Dir, bool i_bInvertX, bool i_bInvertY) const;
	void InvertAnalogDir(int& io_Axis, bool i_bInvertAxis) const;
	void InvertAnalogDir(int& io_X, int& io_Y, bool i_bInvertX, bool i_bInvertY) const;
	void InvertAnalogDir(int& io_X, int& io_Y, int& io_Z, bool i_bInvertX, bool i_bInvertY, bool i_bInvertZ) const;

	//========================================================================
	//	GetKeyboardDigitalDir returns the DigitalDir of the given keyboard
	//========================================================================
	inDigitalDir::DigitalDir GetKeyboardDigitalDir(inKeyboard* i_Keyboard, const DigitalMap& i_Map) const;

	//========================================================================
	//	GetGamepadDigitalDir returns the DigitalDir of the given gamepad
	//========================================================================
	inDigitalDir::DigitalDir GetGamepadDigitalDir(inGamepad* i_Gamepad, const DigitalMap& i_Map) const;

	//========================================================================
	//	AverageDir takes the given vectors of (x,y) 2dvector values and returns 
	//	averaged x, y values as well as float Dir in radians
	//========================================================================
	void AverageDir(std::vector<maVector2d> i_XY, int& o_X, int& o_Y, float& o_Dir) const;

	//========================================================================
	//	ResetConsumed sets all of the consumed flags to false 
	//========================================================================
	void ResetConsumed();

	int m_NumSticks;
	std::map<int, bool> m_HeldConsumed;
	std::map<int, bool> m_PressedConsumed;
	std::map<int, bool> m_ReleasedConsumed;
	std::map<int, bool> m_DirConsumed;
	std::vector<DeviceMap> m_Maps;
	int XMod[inDigitalDir::e_NUMDIRS];
	int YMod[inDigitalDir::e_NUMDIRS];
	maVector2d XYRangedMod[inDigitalDir::e_NUMDIRS];
};