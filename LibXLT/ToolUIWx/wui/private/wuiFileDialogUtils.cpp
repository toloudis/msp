/********************************************************************************************\
**  wuiFileDialogUtils.cpp
**
**      Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "ToolUIWx/wui/wuiFileDialogUtils.hpp"
#include "ToolUIWx/twx/twxWidgets.hpp"

#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"


//----------------------------------------------------------------------------
//	GetOpenFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool wuiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 fsLocator &o_ChosenLocator )
{
#ifdef USE_WXWIDGETS
	wxWindow *pParent = NULL; // could we get pointer to main form here?
	std::string filter_str(i_Filter.c_str());
	std::string init_dir;
	fsFileUtil::LocatorToUnicodeString( i_InitialDir, init_dir );
	wxFileDialog dialog( pParent,
					wxT("Open File"),
					wxString::FromUTF8(init_dir),
					wxEmptyString,
					wxString::FromUTF8(filter_str)
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == wxID_OK)
	{
		fsFileUtil::UnicodeStringToLocator(dialog.GetPath().utf8_string(), o_ChosenLocator);
		return true;
	}
#endif // USE_WXWIDGETS

	return false;
}

//----------------------------------------------------------------------------
//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool wuiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 std::vector<fsLocator> &o_ChosenLocators )
{
#ifdef USE_WXWIDGETS
	wxWindow *pParent = NULL; // could we get pointer to main form here?
	std::string filter_str(i_Filter.c_str());
	std::string init_dir;
	fsFileUtil::LocatorToUnicodeString( i_InitialDir, init_dir );
	wxFileDialog dialog( pParent,
					wxT("Open Files"),
					wxString::FromUTF8(init_dir),
					wxEmptyString,
					wxString::FromUTF8(filter_str),
					wxFD_OPEN|wxFD_MULTIPLE
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == wxID_OK)
	{
		wxArrayString paths;
		dialog.GetPaths(paths);
		o_ChosenLocators.resize(paths.size());
		for (int i=0; i<paths.size(); ++i)
		{
			fsFileUtil::UnicodeStringToLocator(paths[i].utf8_string(), o_ChosenLocators[i]);
		}
		return true;
	}
#endif // USE_WXWIDGETS

	return false;
}

//----------------------------------------------------------------------------
//	GetSaveFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool wuiFileDialogUtils::GetSaveFileName(const std::string &i_Filter,
										 fsLocator &io_ChosenLocator)
{
#ifdef USE_WXWIDGETS
	wxWindow *pParent = NULL; // could we get pointer to main form here?
	std::string filter_str(i_Filter.c_str());
	std::string filename;
	fsFileUtil::LocatorToUnicodeString( io_ChosenLocator, filename );
	wxFileDialog dialog( pParent,
					wxT("Save File"),
					wxEmptyString,
					wxString::FromUTF8(filename),
					wxString::FromUTF8(filter_str),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT
				 );

	//dialog.CentreOnParent();

	if (dialog.ShowModal() == wxID_OK)
	{
		fsFileUtil::UnicodeStringToLocator(dialog.GetPath().utf8_string(), io_ChosenLocator);
		return true;
	}
#endif // USE_WXWIDGETS

	return false;
}