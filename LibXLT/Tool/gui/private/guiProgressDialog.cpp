/*****************************************************************************
**	guiProgressDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiProgressDialog.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//	Show a progress dialog with the given properties. 
//--------------------------------------------------------------------
void guiProgressDialog::Show(const char* i_DialogTitle,
							 const char* i_Message)
{
	DBG_ASSERT(sm_pImplementation, "guiProgressDialog: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->Show(i_DialogTitle, i_Message);
	}
}

//--------------------------------------------------------------------
//  Hide - the dialog is going away
//--------------------------------------------------------------------
void guiProgressDialog::Hide()
{
	DBG_ASSERT(sm_pImplementation, "guiProgressDialog: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->Hide();
	}
}

//--------------------------------------------------------------------
//	Set the percentage and poll for cancellation. Return false if cancelled.
//--------------------------------------------------------------------
bool guiProgressDialog::SetPercentage( float i_Percentage )
{
	DBG_ASSERT(sm_pImplementation, "guiProgressDialog: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->SetPercentage(i_Percentage);
	}
	return true;
}
