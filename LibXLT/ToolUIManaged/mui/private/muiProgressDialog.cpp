/*****************************************************************************
**  muiProgressDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIManaged/mui/muiProgressDialog.hpp"

#include "Core/dbg/dbgLog.hpp"

#include <shlobj.h>

namespace
{

}; // anonymous namespace

//===========================================================================
//	wuiProgressDialog functions
//===========================================================================

//--------------------------------------------------------------------
//	Show a progress dialog with the given properties. 
//--------------------------------------------------------------------
void muiProgressDialog::Show(const char* i_DialogTitle, 
							const char* i_Message)
{
	DBG_ERROR1("ProgressDialog: %s", i_Message);
}

//--------------------------------------------------------------------
//  Hide - the dialog is going away
//--------------------------------------------------------------------
void muiProgressDialog::Hide()
{
}

//--------------------------------------------------------------------
//	Set the percentage and poll for cancellation. Return false if cancelled.
//--------------------------------------------------------------------
bool muiProgressDialog::SetPercentage( float i_Percentage )
{
	return true;
}
