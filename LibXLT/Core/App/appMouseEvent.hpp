/*****************************************************************************
**  appMouseEvent.hpp
**
**      appMouseEvent contains terawatt event types related to mouse
**	activity.  These are appMouseUpEvent, appMouseDownEvent, and
**	appMouseMoveEvent.  There is also a base class appMouseEvent, which
**	contains the position functionality common to all mouse events.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_MOUSEEVENT_HPP
#error appMouseEvent.hpp multiply included
#endif
#define APP_MOUSEEVENT_HPP

#ifndef APP_EVENT_HPP
#include "Core/app/appEvent.hpp"
#endif


//============================================================================
//	appMouseEvent is a base class for the mouse events which contains some
//	common items (like the position)
//============================================================================
class appMouseEvent : public appEvent
{
	public:

		//--------------------------------------------------------------------
		//	The constructor parameters are the client coordinates of the
		//	mouse position when the button operation occurred.
		//--------------------------------------------------------------------
		appMouseEvent(int i_X, int i_Y);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMouseEvent();	

		//--------------------------------------------------------------------
		//	GetX returns the X location of the event in client coordinates.
		//--------------------------------------------------------------------
		int GetX() const;

		//--------------------------------------------------------------------
		//	GetY returns the Y location of the event in client coordinates.
		//--------------------------------------------------------------------
		int GetY() const;

	private:

		int m_X, m_Y;
};

//============================================================================
//	appMouseMoveEvents are sent whenever the mouse is moved.  Get the
//	position with the GetX and GetY functions from appMouseEvent.
//============================================================================
class appMouseMoveEvent : public appMouseEvent
{
	public:

		//--------------------------------------------------------------------
		//	The constructor parameters are the client coordinates of the
		//	final mouse position.
		//--------------------------------------------------------------------
		appMouseMoveEvent(int i_X, int i_Y);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMouseMoveEvent();
};

//============================================================================
//============================================================================
class appMouseUpEvent : public appMouseEvent
{
	public:

		enum Button
		{
			e_Left,
			e_Right,
			e_Middle
		};

		//--------------------------------------------------------------------
		//	The constructor parameters are the client coordinates of the
		//	mouse position when the button operation occurred, and which
		//	button was released.
		//--------------------------------------------------------------------
		appMouseUpEvent(int i_X, int i_Y, Button i_Button);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMouseUpEvent();

		//--------------------------------------------------------------------
		//	GetButton returns the identifier of the button which was released.
		//--------------------------------------------------------------------
		Button GetButton() const;

	private:

		Button m_Button;
};

//============================================================================
//============================================================================
class appMouseDownEvent : public appMouseEvent
{
	public:

		enum Button
		{
			e_Left,
			e_Right,
			e_Middle
		};

		//--------------------------------------------------------------------
		//	The constructor parameters are the client coordinates of the
		//	mouse position when the button operation occurred, and which
		//	button was pressed
		//--------------------------------------------------------------------
		appMouseDownEvent(int i_X, int i_Y, Button i_Button);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~appMouseDownEvent();

		//--------------------------------------------------------------------
		//	GetButton returns the identifier of the button which was pressed.
		//--------------------------------------------------------------------
		Button GetButton() const;

	private:

		Button m_Button;
};
