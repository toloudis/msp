/*****************************************************************************
**	fogDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FOG_DIALOGDATAUTIL_HPP
#error fogDialogDataUtil.hpp multiply included
#endif
#define FOG_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class fogData;
class fogScriptData;


//============================================================================
//============================================================================
class fogDialogDataUtil
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
