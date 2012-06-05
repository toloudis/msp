/*****************************************************************************
**  wuiPropertyDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiPropertyDialog.hpp"

#include "ToolUIWx/twx/twxPropertyDialog.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyDialog::ReturnValue wuiPropertyDialog::ShowModal(const char* i_DialogTitle, 
															const prtyPropertyUIInfoContainer& i_PropertyContainer,
															const char* i_Message)
{
#ifdef USE_WXWIDGETS
	wxString title(i_DialogTitle, wxConvUTF8);
	twxPropertyDialog dialog(twxSystem::g_pMainForm, title, i_PropertyContainer, i_Message);
	int result = dialog.ShowModal();
	if (result == wxID_OK)
		return guiPropertyDialog::e_OK;
	else
		return guiPropertyDialog::e_Cancel;
#else // USE_WXWIDGETS
	return guiPropertyDialog::e_Cancel;
#endif // USE_WXWIDGETS
}

