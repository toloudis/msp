/*****************************************************************************
**  quiFileDialogUtils.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiFileDialogUtils.hpp"

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"

#ifdef USE_QT

namespace
{
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QString convert_filter(const std::string& i_Filter)
	{
		return tqtFileDialogFilter(tqtQString(i_Filter));
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QString locator_to_qstring(const fsLocator& i_Locator)
	{
		std::string path;
		fsFileUtil::LocatorToUnicodeString( i_Locator, path );
		return QString::fromStdString(path);
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void qstring_to_locator(const QString& i_Path, fsLocator& o_Locator)
	{
		std::string path = QDir::toNativeSeparators(i_Path).toStdString();
		fsFileUtil::UnicodeStringToLocator(path, o_Locator);
	}
}

#endif // USE_QT

//---------------------------------------------------------------------------
//	GetOpenFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//---------------------------------------------------------------------------
bool quiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 fsLocator &o_ChosenLocator )
{
#ifdef USE_QT
	QString path = QFileDialog::getOpenFileName( tqtSystem::g_pMainForm,
												 "Open File",
												 locator_to_qstring(i_InitialDir),
												 convert_filter(i_Filter) );
	if (!path.isEmpty())
	{
		qstring_to_locator(path, o_ChosenLocator);
		return true;
	}
#endif // USE_QT
	return false;
}

//---------------------------------------------------------------------------
//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//---------------------------------------------------------------------------
bool quiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 std::vector<fsLocator> &o_ChosenLocators )
{
#ifdef USE_QT
	QStringList paths = QFileDialog::getOpenFileNames( tqtSystem::g_pMainForm,
													   "Open Files",
													   locator_to_qstring(i_InitialDir),
													   convert_filter(i_Filter) );
	if (!paths.isEmpty())
	{
		o_ChosenLocators.resize(paths.size());
		for (int i=0; i<paths.size(); ++i)
		{
			qstring_to_locator(paths[i], o_ChosenLocators[i]);
		}
		return true;
	}
#endif // USE_QT
	return false;
}

//---------------------------------------------------------------------------
//	GetSaveFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//---------------------------------------------------------------------------
bool quiFileDialogUtils::GetSaveFileName(const std::string &i_Filter,
										 fsLocator &io_ChosenLocator)
{
#ifdef USE_QT
	// asks before overwriting an existing file by default
	QString path = QFileDialog::getSaveFileName( tqtSystem::g_pMainForm,
												 "Save File",
												 locator_to_qstring(io_ChosenLocator),
												 convert_filter(i_Filter) );
	if (!path.isEmpty())
	{
		qstring_to_locator(path, io_ChosenLocator);
		return true;
	}
#endif // USE_QT
	return false;
}
