/*****************************************************************************
**	guiPropertyDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiPropertyDialog.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyDialog::ReturnValue guiPropertyDialog::ShowModal(const char* i_DialogTitle, 
															const prtyPropertyUIInfoContainer& i_PropertyContainer,
															const char* i_Message)
{
	DBG_ASSERT(sm_pImplementation, "guiPropertyDialog: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->ShowModal(i_DialogTitle, i_PropertyContainer, i_Message);
	}
	return guiPropertyDialog::e_Cancel;
}

