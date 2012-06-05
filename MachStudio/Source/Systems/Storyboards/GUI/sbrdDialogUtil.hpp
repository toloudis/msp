/*****************************************************************************
**	sbrdDialogUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SBRD_DIALOG_UTIL_HPP
#error sbrdDialogUtil.hpp multiply included
#endif
#define SBRD_DIALOG_UTIL_HPP

#ifndef SBRD_LISTDATA_HPP
#include "Systems/Storyboards/Data/sbrdListData.hpp"
#endif


//============================================================================
//============================================================================
class sbrdDialogUtil
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void Init();

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	static void CleanUp();

	//--------------------------------------------------------------------
	//	create + setup the dialog
	//--------------------------------------------------------------------
	static void SetupDialog();

	//--------------------------------------------------------------------
	// Update individual Storyboard properties dialog
	//--------------------------------------------------------------------
	static void UpdateStoryboardData(int i_Index, const sbrdListData& i_Data);

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of static class
