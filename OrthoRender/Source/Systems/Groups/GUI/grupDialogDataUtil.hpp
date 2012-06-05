/*****************************************************************************
**	grupDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_DIALOGDATAUTIL_HPP
#error grupDialogDataUtil.hpp multiply included
#endif
#define GRUP_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class grupData;
class grupScriptData;


//============================================================================
//============================================================================
class grupDialogDataUtil
{
public:
	//--------------------------------------------------------------------
	//  Update dialog of the list
	//--------------------------------------------------------------------
	static void  UpdateListDialog();

	//--------------------------------------------------------------------
	//	Rebuild the list data
	//--------------------------------------------------------------------
	static void RebuildListData(cmmDialogDataList& io_DataList);

};	// end of static class
