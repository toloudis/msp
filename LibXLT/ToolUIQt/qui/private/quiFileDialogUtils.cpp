/********************************************************************************************\
**  quiFileDialogUtils.cpp
**
**      Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "ToolUIQt/qui/quiFileDialogUtils.hpp"
#include "ToolUIQt/tqt/tqtWidgets.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"


//----------------------------------------------------------------------------
//	GetOpenFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool quiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 fsLocator &o_ChosenLocator )
{
#ifdef QT_FINISH_PORT
	QWidget *pParent = NULL; // could we get pointer to main form here?
	itString filter_str(i_Filter.c_str());
	itString init_dir;
	fsFileUtil::LocatorToUnicodeString( i_InitialDir, init_dir );
	wxFileDialog dialog( pParent,
					_T("Open File"),
					init_dir.GetString(),
					wxEmptyString,
					filter_str.GetString()
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == QMessageBox::Ok)
	{
		fsFileUtil::UnicodeStringToLocator(itString(dialog.GetPath()), o_ChosenLocator);
		return true;
	}
#endif // USE_QT

	return false;
}

//----------------------------------------------------------------------------
//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool quiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 std::vector<fsLocator> &o_ChosenLocators )
{
#ifdef QT_FINISH_PORT
	QWidget *pParent = NULL; // could we get pointer to main form here?
	itString filter_str(i_Filter.c_str());
	itString init_dir;
	fsFileUtil::LocatorToUnicodeString( i_InitialDir, init_dir );
	wxFileDialog dialog( pParent,
					_T("Open Files"),
					init_dir.GetString(),
					wxEmptyString,
					filter_str.GetString(),
					wxFD_OPEN|wxFD_MULTIPLE
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == QMessageBox::Ok)
	{
		wxArrayString paths;
		dialog.GetPaths(paths);
		o_ChosenLocators.resize(paths.size());
		for (int i=0; i<paths.size(); ++i)
		{
			fsFileUtil::UnicodeStringToLocator(itString(paths[i]), o_ChosenLocators[i]);
		}
		return true;
	}
#endif // USE_QT

	return false;
}

//----------------------------------------------------------------------------
//	GetSaveFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool quiFileDialogUtils::GetSaveFileName(const std::string &i_Filter,
										 fsLocator &io_ChosenLocator)
{
#ifdef QT_FINISH_PORT
	QWidget *pParent = NULL; // could we get pointer to main form here?
	itString filter_str(i_Filter.c_str());
	itString filename;
	fsFileUtil::LocatorToUnicodeString( io_ChosenLocator, filename );
	wxFileDialog dialog( pParent,
					_T("Save File"),
					wxEmptyString,
					filename.GetString(),
					filter_str.GetString(),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == QMessageBox::Ok)
	{
		fsFileUtil::UnicodeStringToLocator(itString(dialog.GetPath()), io_ChosenLocator);
		return true;
	}
#endif // USE_QT

	return false;
}