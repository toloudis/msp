/*****************************************************************************
**	lyrsDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LYRS_DIALOGDATAUTIL_HPP
#error lyrsDialogDataUtil.hpp multiply included
#endif
#define LYRS_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class lyrsData;
class lyrsScriptData;


//============================================================================
//============================================================================
class lyrsDialogDataUtil
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
