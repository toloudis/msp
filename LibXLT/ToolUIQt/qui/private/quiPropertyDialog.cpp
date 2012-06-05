/*****************************************************************************
**  quiPropertyDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiPropertyDialog.hpp"

#include "ToolUIQt/tqt/tqtPropertyDialog.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyDialog::ReturnValue quiPropertyDialog::ShowModal(const char* i_DialogTitle, 
															const prtyPropertyUIInfoContainer& i_PropertyContainer,
															const char* i_Message)
{
#ifdef QT_FINISH_PORT
	wxString title(i_DialogTitle, wxConvUTF8);
	tqtPropertyDialog dialog(tqtSystem::g_pMainForm, title, i_PropertyContainer, i_Message);
	int result = dialog.ShowModal();
	if (result == QMessageBox::Ok)
		return guiPropertyDialog::e_OK;
	else
		return guiPropertyDialog::e_Cancel;
#else // USE_WXWIDGETS
	return guiPropertyDialog::e_Cancel;
#endif // USE_QT
}

