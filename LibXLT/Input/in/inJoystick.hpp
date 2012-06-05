/*****************************************************************************
**  inJoystick.hpp
**
**      inJoystick is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_JOYSTICK_HPP
#error inJoystick.hpp multiply included
#endif
#define IN_JOYSTICK_HPP

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif

#ifndef IN_DIGITALDIR_HPP
#include "Input/in/inDigitalDir.hpp"
#endif

// Note:  This is currently implemented as an interface class with no implementation
// in the engine
class inJoystick : public inDevice
{
public:
	enum { e_ANYBUTTON = -1 };

	//========================================================================
	//	Constructor
	//========================================================================
//	inline inJoystick() {}

	//========================================================================
	//	Destructor
	//========================================================================
	virtual inline ~inJoystick() {}

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	virtual void Think()=0;

	//========================================================================
	//	IsPressed
	//========================================================================
	virtual bool IsPressed(int i_Button) const =0;
	virtual bool IsPressed(int i_Button, float& o_AnalogPressure) const =0;

	//========================================================================
	//	IsHeld
	//========================================================================
	virtual bool IsHeld(int i_Button) const =0;
	virtual bool IsHeld(int i_Button, float& o_AnalogPressure) const =0;

	//========================================================================
	//	IsReleased
	//========================================================================
	virtual bool IsReleased(int i_Button) const =0;

	//========================================================================
	//	IsDirPressed returns 0 if there is no direction being pressed on 
	//	i_TerawattStick, else returns 1
	//========================================================================
	virtual bool IsDirPressed(int i_TerawattStick) const=0;

	//========================================================================
	//	GetDigitalDir returns an enumeration value representing the direction
	//	the actual joystick is being pushed in.
	//========================================================================
	virtual inDigitalDir::DigitalDir GetDigitalDir(int i_TerawattStick) const=0;

	//========================================================================
	//	GetAnalogDir returns a float in radians representing the direction the
	//	actual joystick is being pushed in. o_AnalogPressure and o_Twist are
	//	values from 0 to 1 representing how far the stick is being pushed and
	//	far it is being rotated respectively.
	//	The non return value versions give stick location in cartesian style
	//	coordinates.
	//========================================================================
	virtual float GetAnalogDir(int i_TerawattStick) const=0;
	virtual float GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure) const=0;
	virtual float GetAnalogDir(int i_TerawattStick, float& o_AnalogPressure, float& o_Twist) const=0;
	virtual void GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y) const=0;
	virtual void GetAnalogDir(int i_TerawattStick, int& o_x, int& o_y, int& o_z) const=0;

};