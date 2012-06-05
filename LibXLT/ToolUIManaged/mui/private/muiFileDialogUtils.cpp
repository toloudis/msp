/********************************************************************************************\
**  muiFileDialogUtils.cpp
**
**      Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#include "ToolUIManaged/mui/muiFileDialogUtils.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/fs/fsFileUtil.hpp"


//----------------------------------------------------------------------------
//	GetOpenFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool muiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 fsLocator &o_ChosenLocator )
{

	return false;
}

//----------------------------------------------------------------------------
//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool muiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
							  const fsLocator &i_InitialDir,
							  std::vector<fsLocator> &o_ChosenLocators )
{
	return false;
}

//----------------------------------------------------------------------------
//	GetSaveFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool muiFileDialogUtils::GetSaveFileName(const std::string &i_Filter,
										 fsLocator &io_ChosenLocator)
{

	return false;
}