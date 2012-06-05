/*****************************************************************************
**	billDialogUtil.hpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef BILL_DIALOG_UTIL_HPP
#error billDialogUtil.hpp multiply included
#endif
#define BILL_DIALOG_UTIL_HPP

#ifndef BILL_DATA_HPP
#include "Systems/Billboard/Data/billData.hpp"
#endif


//============================================================================
//============================================================================
class billDialogUtil
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
	// Update individual Billboard properties dialog
	//--------------------------------------------------------------------
	static void UpdateBillboardData(int i_Index, const billData& i_Data);

	//--------------------------------------------------------------------
	//	Update the dialog list
	//--------------------------------------------------------------------
	static void UpdateListDialog();

};	// end of static class
