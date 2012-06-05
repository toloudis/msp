/*****************************************************************************
**  wuiPropertyGrid.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiPropertyGrid.hpp"

#include "ToolUIWx/twx/twxPropertyDialog.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyGrid::ReturnValue wuiPropertyGrid::ShowModal(const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows)
{
#ifdef USE_WXWIDGETS
	wxString title(i_DialogTitle, wxConvUTF8);
	twxPropertyDialog dialog(twxSystem::g_pMainForm, title, i_RowNames, i_ColumnNames, i_Rows);
	int result = dialog.ShowModal();
	if (result == wxID_OK)
		return guiPropertyGrid::e_OK;
	else
		return guiPropertyGrid::e_Cancel;
#else // USE_WXWIDGETS
	return guiPropertyGrid::e_Cancel;
#endif // USE_WXWIDGETS
}

