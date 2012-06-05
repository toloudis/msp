/*****************************************************************************
**	envtDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef ENVT_DIALOGUTIL_HPP
#error envtDialogUtil.hpp multiply included
#endif
#define ENVT_DIALOGUTIL_HPP

class envtData;
class envtScriptData;
class nameString;

class envtDialogUtil
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
	//  Show dialog to edit envt
	//--------------------------------------------------------------------
	static void  ShowEnvironmentsDialog();

	//--------------------------------------------------------------------
	// Update individual environment properties dialog
	//--------------------------------------------------------------------
	static void UpdateEnvironmentData(int i_Index, const envtScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual environment properties dialog
	//--------------------------------------------------------------------
	static void UpdateEnvironmentData(int i_Index, const envtData& i_Data);

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	static void ClearDialog();
	static void UpdateDialog(const nameString& i_EnvironmentName);

	//--------------------------------------------------------------------
	// Update specific pages when membership changed from something
	// outside GUI checkboxes
	//--------------------------------------------------------------------
	static void UpdateObjectsPage();
	
	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	static void AddDataPage();
	static void RemoveDataPage();
};

