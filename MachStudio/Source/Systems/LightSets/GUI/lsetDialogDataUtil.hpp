/*****************************************************************************
**	lsetDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_DIALOGDATAUTIL_HPP
#error lsetDialogDataUtil.hpp multiply included
#endif
#define LSET_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

//============================================================================
//============================================================================
class lsetData;
class lsetScriptData;


//============================================================================
//============================================================================
class lsetDialogDataUtil
{
public:
	//--------------------------------------------------------------------
	//  Update dialog of the list
	//--------------------------------------------------------------------
	static void  UpdateListDialog();

	//--------------------------------------------------------------------
	//  Update tree view of light sets
	//--------------------------------------------------------------------
	static void  UpdateSetRelationships();

	//--------------------------------------------------------------------
	//	Rebuild the list data
	//--------------------------------------------------------------------
	static void RebuildListData(cmmDialogDataList& io_DataList);

};	// end of static class
