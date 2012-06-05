/*****************************************************************************
**  inMouse.hpp
**
**      inMouse is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef IN_MOUSE_HPP
#error inMouse.hpp multiply included
#endif
#define IN_MOUSE_HPP

#ifndef IN_DEVICE_HPP
#include "Input/in/inDevice.hpp"
#endif

#ifndef IN_DIGITALDIR_HPP
#include "Input/in/inDigitalDir.hpp"
#endif

//========================================================================
//	Forward References
//========================================================================
class inMousePAC;

class inMouse : public inDevice
{
public:
	enum { e_ANYBUTTON = -1 };

	//========================================================================
	//	Constructor
	//========================================================================
	inMouse();

	//========================================================================
	//	Destructor
	//========================================================================
	~inMouse();

	//========================================================================
	//	Think gives the device a chance to update its state once per frame
	//========================================================================
	void Think();

	//========================================================================
	//	IsPressed
	//========================================================================
	bool IsPressed(int i_Button);
	bool IsPressed(int i_Button, float& o_AnalogPressure);

	//========================================================================
	//	IsHeld
	//========================================================================
	bool IsHeld(int i_Button);
	bool IsHeld(int i_Button, float& o_AnalogPressure);

	//========================================================================
	//	IsReleased
	//========================================================================
	bool IsReleased(int i_Button);

	//========================================================================
	//	GetPosition returns the relative change in mouse position since
	//	the last frame, and optionally mouse wheel rotation.
	//========================================================================
	void GetPosition(int& o_dX, int& o_dY);
	void GetPosition(int& o_dX, int& o_dY, float& o_dZ);	

	//========================================================================
	//	IsDirPressed returns 0 if there is no direction being pressed on 
	//	i_Stick, else returns 1.  The stick is left for compliance with these calls in
	//	other devices as there is no real concept of a stick for the mouse
	//========================================================================
	bool IsDirPressed(int i_Stick) const;

	//========================================================================
	//	GetDigitalDir returns the current digital direction for i_Stick.  The 
	//	stick is left for compliance with these calls in
	//	other devices as there is no real concept of a stick for the mouse
	//========================================================================
	inDigitalDir::DigitalDir GetDigitalDir(int i_Stick) const;

	//========================================================================
	//	GetAnalogDir returns i_Sticks' direction in various configurations
	//	the float returned and o_Twist are direction in radians, and 
	//	o_AnalogPressure is a value from 0 - 1 representing how far the stick
	//	is being pushed. The stick is left for compliance with these calls in
	//	other devices as there is no real concept of a stick for the mouse
	//========================================================================
	float GetAnalogDir(int i_Stick) const;
	float GetAnalogDir(int i_Stick, float& o_AnalogPressure) const;
	float GetAnalogDir(int i_Stick, float& o_AnalogPressure, float& o_Twist) const;
	void GetAnalogDir(int i_Stick, int& o_X, int& o_Y) const;
	void GetAnalogDir(int i_Stick, int& o_X, int& o_Y, int& o_Z) const;

private:
	enum { e_MOUSEBUFSIZE = 8 };
	//we'll arbitrarily set this at 50, it seems around the limit of motion when not trying too hard
	//so it should be a fine range for determining the analog pressure
	enum { e_MOUSERANGE = 50 };
	int m_dX;
	int m_dY;
	float m_dZ;
	float m_Dir;
	float m_Pressure;
	inDigitalDir::DigitalDir m_DigitalDir;
	bool m_PastState[e_MOUSEBUFSIZE];
	bool m_CurState[e_MOUSEBUFSIZE];
	inMousePAC *m_pPAC;
};