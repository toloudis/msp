/*****************************************************************************
**  inVirtualJoystick.hpp
**
**      inVirtualJoystick provides an abstract base class of key virtual joystick
**		functions such as IsPressed, but no functions involving mappings, that is
**		to the more implementation specific derived classes
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_VIRTUALJOYSTICK_HPP
#error inVirtualJoystick.hpp multiply included
#endif
#define IN_VIRTUALJOYSTICK_HPP

#ifndef IN_DIGITALDIR_HPP
#include "Input/in/inDigitalDir.hpp"
#endif

class inVirtualJoystick
{
public:
	//========================================================================
	//	enums
	//========================================================================
	enum { e_ANYBUTTON = -1 };
	enum { e_NUMSTICKS = 5 };

	//========================================================================
	//	Destructor
	//========================================================================
	virtual ~inVirtualJoystick() = 0;

	//========================================================================
	//	IsPressed returns true if any of the devices registered currently have
	//	a button pressed that maps to i_TerawattButton. Can also return a value
	//	from 0 to 1 in o_AnalogPressure representing how far the button is being
	//	pushed. Pass in e_ANYBUTTON to check if any button is pressed, Not a 
	//	valid parameter when requesting pressure info.
	//========================================================================
	virtual bool IsPressed(int i_TerawattButton) const = 0;
	virtual bool IsPressed(int i_TerawattButton, float& o_AnalogPressure) const = 0;

	//========================================================================
	//	IsHeld returns true if any of the devices registered currently have and
	//	also had a button pressed that maps to i_TerawattButton. Can also return
	//	a value from 0 to 1 in o_AnalogPressure representing how far the button
	//	is being pushed. Pass in e_ANYBUTTON to check if any button is pressed, Not a 
	//	valid parameter when requesting pressure info.
	//========================================================================
	virtual bool IsHeld(int i_TerawattButton) const = 0;
	virtual bool IsHeld(int i_TerawattButton, float& o_AnalogPressure) const = 0;

	//========================================================================
	//	IsReleased returns true if any of the devices registered currently have
	//	a button pressed that maps to i_TerawattButton. Pass in e_ANYBUTTON to
	//	check if any button is pressed.
	//========================================================================
	virtual bool IsReleased(int i_TerawattButton) const = 0;

	//========================================================================
	//	IsDirPressed returns 0 if there is no direction being pressed on 
	//	i_TerawattStick, else returns 1
	//========================================================================
	virtual bool IsDirPressed(int i_TerawattStick) const = 0;

	//========================================================================
	//	GetDigitalDir returns an enumeration value representing the direction
	//	the actual joystick is being pushed in.
	//========================================================================
	virtual inDigitalDir::DigitalDir GetDigitalDir(int i_TerawattStick) const = 0;

	//========================================================================
	//	GetAnalogDir returns a float in radians representing the direction the
	//	actual joystick is being pushed in. o_AnalogPressure and o_Twist are
	//	values from 0 to 1 representing how far the stick is being pushed and
	//	far it is being rotated respectively.
	//	The non return value versions give stick location in cartesian style
	//	coordinates.
	//========================================================================
	virtual float GetAnalogDir(int i_TerawattStick) const = 0;
	virtual float GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure) const = 0;
	virtual float GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure, float& o_Twist) const = 0;
	virtual void GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y) const = 0;
	virtual void GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y, int& o_z) const = 0;

	//========================================================================
	//	GetAnalogAxes returns a representation of the position of the joystick
	//	along the x and y axes from -1 to 1, great for when you want the
	//	direction and intensity of the joystick in terms of x and y
	//	North and West (ie Up and Left on a joystick) are negative, South and
	//	East are positive
	//========================================================================
	virtual void GetAnalogAxes(int i_TerawattStick, float& o_x, float& o_y) const = 0;

	//========================================================================
	//	Think resets all consumed maps
	//========================================================================
	virtual void Think() = 0;
};

//========================================================================
//========================================================================
inline inVirtualJoystick::~inVirtualJoystick()
{
}
