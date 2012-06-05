/*****************************************************************************
**	lsetDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef LSET_DIALOGUTIL_HPP
#error lsetDialogUtil.hpp multiply included
#endif
#define LSET_DIALOGUTIL_HPP


class lsetData;
class lsetScriptData;
class lsetScriptObject;
class nameString;

class lsetDialogUtil
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
	//  Show dialog to edit lset
	//--------------------------------------------------------------------
	static void  ShowLightSetsDialog();

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	static void ClearDialog();
	static void UpdateDialog(const nameString &i_LightSetName);

	//--------------------------------------------------------------------
	// Update specific pages when membership changed from something
	// outside GUI checkboxes
	//--------------------------------------------------------------------
	static void UpdateObjectsPage();
	static void UpdateLightsPage();
	
	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	static void AddDataPage();
	static void RemoveDataPage();
};

