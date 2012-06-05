/*****************************************************************************
**	dcutDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef DCUT_DIALOGUTIL_HPP
#error dcutDialogUtil.hpp multiply included
#endif
#define DCUT_DIALOGUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class dcutCueData;
class dcutScriptData;


//============================================================================
//============================================================================
class dcutDialogUtil
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
	// Update individual camera properties dialog
	//--------------------------------------------------------------------
	static void UpdateCameraData(int i_Index, const dcutCueData& i_Data);

	//--------------------------------------------------------------------
	// Update individual camera properties dialog
	//--------------------------------------------------------------------
	static void UpdateCameraData(int i_Index, const dcutScriptData& i_Data);

	//--------------------------------------------------------------------
	//	Select no camera
	//--------------------------------------------------------------------
	static void DeselectCamera();

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();


};	// end of static class

