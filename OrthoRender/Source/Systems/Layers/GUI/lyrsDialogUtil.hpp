/*****************************************************************************
**	lyrsDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef LYRS_DIALOGUTIL_HPP
#error lyrsDialogUtil.hpp multiply included
#endif
#define LYRS_DIALOGUTIL_HPP

class lyrsLayerObject;

class lyrsDialogUtil
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
	//  Show dialog to edit lyrs
	//--------------------------------------------------------------------
	static void  ShowLayersDialog();

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	static void UpdateDialog(lyrsLayerObject *i_pObject);
	
	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	static void AddDataPage();
	static void RemoveDataPage();
};

