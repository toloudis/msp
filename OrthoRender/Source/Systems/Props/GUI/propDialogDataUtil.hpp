/*****************************************************************************
**	propDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_DIALOGDATAUTIL_HPP
#error propDialogDataUtil.hpp multiply included
#endif
#define PROP_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class propData;
class propScriptData;


//============================================================================
//============================================================================
class propDialogDataUtil
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
