/*****************************************************************************
**	dirltDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef DIRLT_DIALOGUTIL_HPP
#error dirltDialogUtil.hpp multiply included
#endif
#define DIRLT_DIALOGUTIL_HPP


//============================================================================
//============================================================================
class dirltData;
class dirltScriptData;


//============================================================================
//============================================================================
class dirltDialogUtil
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
	//  Show dialog to add/delete dir lights
	//--------------------------------------------------------------------
	//static void  ShowDirLightsDialog();

	//--------------------------------------------------------------------
	//  Show dialog to set individual properties of a dir light
	//--------------------------------------------------------------------
	//static void  ShowDirLightDataDialog();

	//--------------------------------------------------------------------
	// Update main list dialog
	//--------------------------------------------------------------------
	static void UpdateListDialog();

	//--------------------------------------------------------------------
	// Update individual dir light properties dialog
	//--------------------------------------------------------------------
	static void UpdateLightData(int i_Index, const dirltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual dir light properties dialog
	//--------------------------------------------------------------------
	static void UpdateLightData(int i_Index, const dirltScriptData& i_Data);

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	static void  AddDataPage();
	static void  RemoveDataPage();

	//--------------------------------------------------------------------
	// Light Data can only be displayed when a light is selected.
	//	When a light is not selected, call this function to disable
	//	the interface.
	//--------------------------------------------------------------------
//	static void DisableLightDataDialog();

};	// end of static class

