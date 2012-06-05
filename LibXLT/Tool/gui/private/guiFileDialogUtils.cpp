/********************************************************************************************\
**	guiFileDialogUtils.cpp
**
**		Utilities for opening/saving files using fsLocators
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Tool/gui/guiFileDialogUtils.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsLocator.hpp"
#include "Core/Gf/gfDirectoryCategories.hpp"


//----------------------------------------------------------------------------
//	GetOpenFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool guiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
										 const fsLocator &i_InitialDir,
										 fsLocator &o_ChosenLocator )
{
	DBG_ASSERT(sm_pImplementation, "guiFileDialogUtils: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetOpenFileName(i_Filter, i_InitialDir, o_ChosenLocator);
	}
	return false;
}

//----------------------------------------------------------------------------
// This variation of GetOpenFileName uses the directory category to set
// the initial directory and updates the directory category when a file 
// is chosen.
//----------------------------------------------------------------------------
bool guiFileDialogUtils::GetOpenFileName(const std::string &i_Filter,
					 const std::string &i_DirectoryCategory,
					 fsLocator &o_ChosenLocator )
{
	fsLocator browsePath; 
	gfDirectoryCategories::GetCurDirectory(i_DirectoryCategory, browsePath);

	if (GetOpenFileName(i_Filter, browsePath, o_ChosenLocator))
	{
		// Set current directory to use for further dialogs using the
		// same category
		if (o_ChosenLocator.GetNumNames() > 1)
		{
			fsLocator cur_dir = o_ChosenLocator;
			cur_dir.Pop();
			gfDirectoryCategories::SetCurDirectory(i_DirectoryCategory, cur_dir);
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//	GetOpenFileNames() - returns multiple locators in o_ChosenLocators.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool guiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
							 const fsLocator &i_InitialDir,
							 std::vector<fsLocator> &o_ChosenLocators )
{
	DBG_ASSERT(sm_pImplementation, "guiFileDialogUtils: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetOpenFileNames(i_Filter, i_InitialDir, o_ChosenLocators);
	}
	return false;
}

//----------------------------------------------------------------------------
// This variation of GetOpenFileName uses the directory category to set
// the initial directory and updates the directory category when a file 
// is chosen.
//----------------------------------------------------------------------------
bool guiFileDialogUtils::GetOpenFileNames(const std::string &i_Filter,
					 const std::string &i_DirectoryCategory,
					 std::vector<fsLocator> &o_ChosenLocators )
{
	fsLocator browsePath; 
	gfDirectoryCategories::GetCurDirectory(i_DirectoryCategory, browsePath);

	if (GetOpenFileNames(i_Filter, browsePath, o_ChosenLocators))
	{
		if (!o_ChosenLocators.empty())
		{
			// Set current directory to use for further dialogs using the
			// same category
			fsLocator cur_dir = o_ChosenLocators[0];
			if (cur_dir.GetNumNames() > 1)
			{
				cur_dir.Pop();
				gfDirectoryCategories::SetCurDirectory(i_DirectoryCategory, cur_dir);
			}
		}
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//	GetSaveFileName() - returns result in o_ChosenLocator.
//		returns true if a file was chosen, false if cancelled
//		was pressed.
//	i_Filter should be string like: "My files (*.myf)|*.myf|All files (*.*)|*.*"
//----------------------------------------------------------------------------
bool guiFileDialogUtils::GetSaveFileName(const std::string &i_Filter,
										 fsLocator &io_ChosenLocator)
{
	DBG_ASSERT(sm_pImplementation, "guiFileDialogUtils: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->GetSaveFileName(i_Filter, io_ChosenLocator);
	}
	return false;
}
