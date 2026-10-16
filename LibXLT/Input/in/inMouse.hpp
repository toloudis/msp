/*****************************************************************************
**  inMouse.hpp
**
**      inMouse is derived from inDevice. It provides a Think function to poll
**		for its current state, as well as appropriate accessors for retrieving
**		info about the state.
**
**		The mouse has no device of its own. The GUI toolkit supplies an
**		inMouseSource that Think samples for the cursor position and
**		buttons, and reports button presses (NotifyButtonPressed) and wheel
**		rotation (AddWheelDelta) from its events as they arrive.
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
//	inMouseSource is how inMouse reads the cursor and buttons
//========================================================================
class inMouseSource
{
public:
	virtual ~inMouseSource() {}

	//========================================================================
	//	Sample returns false when the application doesn't have the mouse
	//	(it isn't the active application). Otherwise it returns the
	//	cursor position in screen pixels and the buttons held down, as a
	//	bit mask: bit 0 left, 1 right, 2 middle, 3 and 4 the extra buttons.
	//========================================================================
	virtual bool Sample(int& o_ScreenX, int& o_ScreenY, unsigned int& o_Buttons) = 0;
};

class inMouse : public inDevice
{
public:
	enum { e_ANYBUTTON = -1 };
	enum { e_LEFT = 0, e_RIGHT = 1, e_MIDDLE = 2, e_EXTRA1 = 3, e_EXTRA2 = 4 };

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

	//========================================================================
	//	SetSource sets where Think reads the cursor and buttons from. The
	//	mouse doesn't own it. Pass NULL before deleting the source.
	//========================================================================
	void SetSource(inMouseSource* i_pSource);

	//========================================================================
	//	ResetMotion forgets the last cursor position, so the next Think
	//	reports no motion. Call it when the application gets or loses the
	//	mouse, so motion made while away doesn't arrive all at once.
	//========================================================================
	void ResetMotion();

	//========================================================================
	//	NotifyButtonPressed reports a button press event, so that a click
	//	that goes down and back up between two Thinks still shows as
	//	pressed for a frame.
	//========================================================================
	void NotifyButtonPressed(int i_Button);

	//========================================================================
	//	AddWheelDelta reports wheel rotation, in notches, from the window
	//	under the cursor. It is applied on the next Think.
	//========================================================================
	void AddWheelDelta(float i_Notches);

private:
	enum { e_MOUSEBUFSIZE = 8 };
	//we'll arbitrarily set this at 50, it seems around the limit of motion when not trying too hard
	//so it should be a fine range for determining the analog pressure
	enum { e_MOUSERANGE = 50 };
	int m_dX;
	int m_dY;
	float m_dZ;
	float m_PendingWheel;
	float m_Dir;
	float m_Pressure;
	inDigitalDir::DigitalDir m_DigitalDir;
	bool m_PastState[e_MOUSEBUFSIZE];
	bool m_CurState[e_MOUSEBUFSIZE];
	bool m_PressedSinceThink[e_MOUSEBUFSIZE];
	inMouseSource *m_pSource;
	bool m_bHaveLastPos;	// false until the first sample, and after losing the mouse
	int m_LastX;
	int m_LastY;
};