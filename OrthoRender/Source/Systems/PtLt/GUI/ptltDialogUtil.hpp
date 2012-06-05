/*****************************************************************************
**	ptltDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_DIALOGUTIL_HPP
#error ptltDialogUtil.hpp multiply included
#endif
#define PTLT_DIALOGUTIL_HPP


//============================================================================
//============================================================================
class ptltData;
class ptltScriptData;


//============================================================================
//============================================================================
class ptltDialogUtil
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
	// Update individual point light properties dialog
	//--------------------------------------------------------------------
	static void UpdateLightData(int i_Index, const ptltScriptData& i_Data);

	//--------------------------------------------------------------------
	// Update individual point light properties dialog
	//--------------------------------------------------------------------
	static void UpdateLightData(int i_Index, const ptltData& i_Data);

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of static class
