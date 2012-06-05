/*****************************************************************************
**	billDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_DIALOGDATAUTIL_HPP
#error billDialogDataUtil.hpp multiply included
#endif
#define BILL_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class billData;
class billScriptData;


//============================================================================
//============================================================================
class billDialogDataUtil
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
