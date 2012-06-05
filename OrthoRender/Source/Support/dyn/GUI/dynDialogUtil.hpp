/*****************************************************************************
**	dynDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#ifdef DYN_DIALOGUTIL_HPP
#error dynDialogUtil.hpp multiply included
#endif
#define DYN_DIALOGUTIL_HPP

class dynScriptObject;

namespace dynDialogUtil
{

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(dynScriptObject *i_pObject);

	//--------------------------------------------------------------------
	// Notify that channel values may have changed.
	//--------------------------------------------------------------------
	void UpdateControlData(dynScriptObject *i_pObject);

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage();
	void  RemoveDataPage();

}	// end of namespace
