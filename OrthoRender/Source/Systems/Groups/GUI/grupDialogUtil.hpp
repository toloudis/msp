/*****************************************************************************
**	grupDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef GRUP_DIALOGUTIL_HPP
#error grupDialogUtil.hpp multiply included
#endif
#define GRUP_DIALOGUTIL_HPP

class grupGroupObject;

class grupDialogUtil
{
public:
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	static void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	static void  CleanUp();

	//--------------------------------------------------------------------
	//  Show dialog to edit grup
	//--------------------------------------------------------------------
	static void  ShowGroupsDialog();

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	static void UpdateDialog(grupGroupObject *i_pObject);
	
	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	static void AddDataPage();
	static void RemoveDataPage();
};

