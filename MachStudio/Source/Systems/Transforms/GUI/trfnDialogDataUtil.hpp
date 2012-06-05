/*****************************************************************************
**	trfnDialogDataUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TRFN_DIALOGDATAUTIL_HPP
#error trfnDialogDataUtil.hpp multiply included
#endif
#define TRFN_DIALOGDATAUTIL_HPP

#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif


//============================================================================
//============================================================================
class trfnData;
class trfnScriptData;


//============================================================================
//============================================================================
class trfnDialogDataUtil
{
public:
	//--------------------------------------------------------------------
	//  Update dialog of the list
	//--------------------------------------------------------------------
	static void  UpdateListDialog();

	//--------------------------------------------------------------------
	//  Update tree view of scene hierarchy
	//--------------------------------------------------------------------
	static void UpdateSceneHierarchy();

	//--------------------------------------------------------------------
	//	Rebuild the list data
	//--------------------------------------------------------------------
	static void RebuildListData(cmmDialogDataList& io_DataList);

};	// end of static class
