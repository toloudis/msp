/*****************************************************************************
**	setsDialogUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef SETS_DIALOG_UTIL_HPP
#error setsDialogUtil.hpp multiply included
#endif
#define SETS_DIALOG_UTIL_HPP


namespace setsDialogUtil
{

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	//  Show dialog to add/remove set items
	//--------------------------------------------------------------------
	void ShowSetItemsDialog();

	//--------------------------------------------------------------------
	//	Rebuild the list dialog
	//--------------------------------------------------------------------
	void RebuildListDialog();

}	// end of namespace
