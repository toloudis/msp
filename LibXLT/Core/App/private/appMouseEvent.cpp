/*****************************************************************************
**  appMouseEvent.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appMouseEvent.hpp"


//============================================================================
//	appMouseEvent is a base class for the mouse events which contains some
//	common items (like the position)
//============================================================================
//--------------------------------------------------------------------
//	The constructor parameters are the client coordinates of the
//	mouse position when the button operation occurred, and the 
//--------------------------------------------------------------------
appMouseEvent::appMouseEvent(int i_X, int i_Y)
:	m_X(i_X),
	m_Y(i_Y)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseEvent::~appMouseEvent()	
{
}

//--------------------------------------------------------------------
//	GetX returns the X location of the event in client coordinates.
//--------------------------------------------------------------------
int appMouseEvent::GetX() const
{
	return m_X;
}

//--------------------------------------------------------------------
//	GetY returns the Y location of the event in client coordinates.
//--------------------------------------------------------------------
int appMouseEvent::GetY() const
{
	return m_Y;
}


//============================================================================
//	appMouseMoveEvents are sent whenever the mouse is moved.  Get the
//	position with the GetX and GetY functions from appMouseEvent.
//============================================================================
//--------------------------------------------------------------------
//	The constructor parameters are the client coordinates of the
//	final mouse position.
//--------------------------------------------------------------------
appMouseMoveEvent::appMouseMoveEvent(int i_X, int i_Y)
:	appMouseEvent(i_X, i_Y)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseMoveEvent::~appMouseMoveEvent()
{
}


//============================================================================
//============================================================================
//--------------------------------------------------------------------
//	The constructor parameters are the client coordinates of the
//	mouse position when the button operation occurred, and which
//	button was released.
//--------------------------------------------------------------------
appMouseUpEvent::appMouseUpEvent(int i_X, int i_Y, Button i_Button)
:	appMouseEvent(i_X, i_Y),
	m_Button(i_Button)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseUpEvent::~appMouseUpEvent()
{
}

//--------------------------------------------------------------------
//	GetButton returns the identifier of the button which was released.
//--------------------------------------------------------------------
appMouseUpEvent::Button appMouseUpEvent::GetButton() const
{
	return m_Button;
}

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//	The constructor parameters are the client coordinates of the
//	mouse position when the button operation occurred, and which
//	button was pressed
//--------------------------------------------------------------------
appMouseDownEvent::appMouseDownEvent(int i_X, int i_Y, Button i_Button)
:	appMouseEvent(i_X, i_Y),
	m_Button(i_Button)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
appMouseDownEvent::~appMouseDownEvent()
{
}

//--------------------------------------------------------------------
//	GetButton returns the identifier of the button which was pressed.
//--------------------------------------------------------------------
appMouseDownEvent::Button appMouseDownEvent::GetButton() const
{
	return m_Button;
}
