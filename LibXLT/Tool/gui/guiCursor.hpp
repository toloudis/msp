/****************************************************************************\
**	guiCursor.hpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_CURSOR_HPP
#error guiCursor.hpp multiply included
#endif
#define GUI_CURSOR_HPP

#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiCursorImpl;


//============================================================================
//============================================================================
class guiCursor: public envAbstraction<guiCursorImpl>
{
public:

	static void SetWaitCursor();
	static void EndWaitCursor();
};


//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiCursorImpl
{
public:
	virtual void SetWaitCursor() = 0;
	virtual void EndWaitCursor() = 0;
};