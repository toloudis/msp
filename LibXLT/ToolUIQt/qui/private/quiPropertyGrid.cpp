/*****************************************************************************
**  quiPropertyGrid.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiPropertyGrid.hpp"

#include "ToolUIQt/tqt/tqtPropertyDialog.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyGrid::ReturnValue quiPropertyGrid::ShowModal(const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows)
{
#ifdef USE_QT
	QString title = tqtQString(i_DialogTitle);
	tqtPropertyDialog dialog(tqtSystem::g_pMainForm, title, i_RowNames, i_ColumnNames, i_Rows);
	int result = dialog.exec();
	if (result == QDialog::Accepted)
		return guiPropertyGrid::e_OK;
	else
		return guiPropertyGrid::e_Cancel;
#else // USE_QT
	return guiPropertyGrid::e_Cancel;
#endif // USE_QT
}

