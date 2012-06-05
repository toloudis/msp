/*****************************************************************************
**	prjltDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PRJLT_DIALOGUTIL_HPP
#error prjltDialogUtil.hpp multiply included
#endif
#define PRJLT_DIALOGUTIL_HPP


//============================================================================
//	forward references
//============================================================================
class prjltData;
class prjltScriptData;


//============================================================================
//============================================================================
class prjltDialogUtil
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
	// Update individual projected light properties dialog
	//--------------------------------------------------------------------
	static void UpdateData(int i_Index, const prjltData& i_Data);

	//--------------------------------------------------------------------
	// Update individual projected light properties dialog
	//--------------------------------------------------------------------
	static void UpdateLightData(int i_Index, const prjltScriptData& i_Data);

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of static class

