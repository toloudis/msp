/*****************************************************************************
**	chtrDialogUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CHTR_DIALOG_UTIL_HPP
#error chtrDialogUtil.hpp multiply included
#endif
#define CHTR_DIALOG_UTIL_HPP

#ifndef CHTR_SCRIPTDATA_HPP
#include "Systems/Character/Data/chtrScriptData.hpp"
#endif


//============================================================================
//============================================================================
class chtrData;
class chtrScriptData;


//============================================================================
//============================================================================
class chtrDialogUtil
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void Init();

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	static void CleanUp();

	//--------------------------------------------------------------------
	// Update individual properties dialog
	//--------------------------------------------------------------------
	static void UpdateCharacterData(int i_Index, const chtrData& i_Data);

	//--------------------------------------------------------------------
	// Character Data can only be displayed when a Character is selected.
	//	When a Character is not selected, call this function to disable
	//	the interface.
	//--------------------------------------------------------------------
	static void DisableCharacterDataDialog();

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of static class

