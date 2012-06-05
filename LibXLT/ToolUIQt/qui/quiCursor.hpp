/****************************************************************************\
**	quiCursor.hpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_CURSOR_HPP
#error quiCursor.hpp multiply included
#endif
#define QUI_CURSOR_HPP

#ifndef GUI_CURSOR_HPP
#include "Tool/gui/guiCursor.hpp"
#endif

//============================================================================
//============================================================================
class quiCursor : public guiCursorImpl
{

	virtual	void SetWaitCursor() ;
	virtual void EndWaitCursor() ;
		
};



