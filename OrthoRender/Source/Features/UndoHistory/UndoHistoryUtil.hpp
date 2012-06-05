/*****************************************************************************
**	UndoHistoryUtil.hpp
**
**		API for UndoHistory utilities
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef UNDOHISTORYUTIL_HPP
#error UndoHistoryUtil.hpp multiply included
#endif
#define UNDOHISTORYUTIL_HPP

#ifndef UNDOHISTORYDATA_HPP
#include "Features/UndoHistory/UndoHistoryData.hpp"
#endif


//============================================================================
//============================================================================
namespace UndoHistoryUtil
{
	//------------------------------------------------------------------------
	//  AddToMenu() - add UndoHistory actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu();

	//------------------------------------------------------------------------
	//	Build a data list
	//------------------------------------------------------------------------
	void BuildDataList( UndoHistoryData& o_Data );
}
