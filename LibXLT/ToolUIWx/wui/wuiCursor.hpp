/****************************************************************************\
**	wuiCursor.hpp
**
**	class to invloke wxCursor
**	
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef WUI_CURSOR_HPP
#error wuiCursor.hpp multiply included
#endif
#define WUI_CURSOR_HPP

#ifndef GUI_CURSOR_HPP
#include "Tool/gui/guiCursor.hpp"
#endif

//============================================================================
//============================================================================
class wuiCursor : public guiCursorImpl
{

	virtual	void SetWaitCursor() ;
	virtual void EndWaitCursor() ;
		
};



