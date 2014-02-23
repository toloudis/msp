/****************************************************************************\
**	mainResolvePath.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainResolvePath.hpp"
#include "MainApp/wxGUI/wxResolvePathDialog.hpp"

#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <set>


//============================================================================
//============================================================================
namespace
{
	// When not using a GUI, what should the interface do when a file is not found?
	// If l_bStrictMode is true, it will throw a file not found exception which will
	// cause the load to abort.
	bool l_bStrictMode = true;

	// SkipAll can be set to true by the user's choice. This
	// means return "Skip" to all unfound files without asking anymore.
	//bool l_bSkipAll = false;
	std::set<std::string> l_SkippedCategories;
	std::set<fsLocator> l_SkippedFiles;

}	// end of namespace

//------------------------------------------------------------------------
// Add our function as handler
//------------------------------------------------------------------------
void mainResolvePath::Init()
{
	// Add application's handler to library's manager
	fsAbsolutePathMgr::SetPathResolveFunction(mainResolvePath::ResolvePath);
}

//------------------------------------------------------------------------
// Clear out mapping data from the current file load
//------------------------------------------------------------------------
void mainResolvePath::LoadFinished()
{
	// reset the skip all bit so that new assets are searched for again
	//l_bSkipAll = false;
	l_SkippedCategories.clear();
	l_SkippedFiles.clear();
}

//------------------------------------------------------------------------
// ResolvePath() - the given filename cannot be found, ask user how to
//	handle it. Returns true if a new file is chosen, returning the new
//	filename in o_LocalFilename.
//------------------------------------------------------------------------
bool mainResolvePath::ResolvePath(const fsLocator& i_OrigFilename, 
								  fsLocator& o_LocalFilename,
								  const std::string &i_Category)
{
#ifdef USE_WXWIDGETS
	DBG_WARNING("Cannot find file: " << i_OrigFilename);
	if (twxSystem::g_pMainForm)
	{
		// If we already skipped this file, or if we are skipping all files, then
		// don't bother asking again.
		bool bAskUser = ((l_SkippedCategories.find(i_Category) == l_SkippedCategories.end()) && 
						 (l_SkippedFiles.find(i_OrigFilename) == l_SkippedFiles.end()));
		while (bAskUser)
		{
			wxResolvePathDialog dialog(twxSystem::g_pMainForm, i_OrigFilename,i_Category);
			dialog.ShowModal();
			switch (dialog.GetResolveChoice())
			{
			default:
			case wxResolvePathDialog::e_Retry:
				// Retry implies that the user put the asset into place and is ready to 
				// try reloading it. If this is true, then return true. Otherwise,
				// ask user again.
				if (fsFileUtil::FileExists(i_OrigFilename))
				{
					o_LocalFilename = i_OrigFilename;
					return true;
				}
				break;
			case wxResolvePathDialog::e_SkipAll:
				// Set flag such that next file that is not found returns false without
				// asking the user anymore.
				//l_bSkipAll = true;
				l_SkippedCategories.insert(i_Category);
				return false;
			case wxResolvePathDialog::e_Skip:
				l_SkippedFiles.insert(i_OrigFilename);
				return false;
			case wxResolvePathDialog::e_Locate:
				{
					//fsLocator init_dir = i_OrigFilename;
					//if (init_dir.GetNumNames() > 0)
					//	init_dir.Pop();
					//if (guiFileDialogUtils::GetOpenFileName("All files (*.*)|*.*", init_dir, o_LocalFilename))
					itString fullpath;
					fsFileUtil::LocatorToUnicodeString( i_OrigFilename, fullpath );
					itString title("Locate File: ");
					title += i_OrigFilename.GetLastName();
					wxFileDialog file_dialog( twxSystem::g_pMainForm,
									title.GetString(),
									wxEmptyString,
									fullpath.GetString(),
									_T("*.*"),
									wxFD_DEFAULT_STYLE|wxFD_FILE_MUST_EXIST
								 );

					//file_dialog.CentreOnParent();

					if (file_dialog.ShowModal() == wxID_OK)
					{
						itString full_path((const char*)file_dialog.GetPath().c_str());
						fsFileUtil::UnicodeStringToLocator(full_path, o_LocalFilename);
						if (o_LocalFilename.GetNumNames() > 0)
						{
							// Add file mapping so that we can use the user's choice to find other files
							fsAbsolutePathMgr::AddFileMapping(i_OrigFilename, o_LocalFilename);
							DBG_LOG("Replacing file: " << i_OrigFilename << " with: " << o_LocalFilename);
							return true;
						}
					}
					// If user chooses "Cancel" from file selection dialog,
					// continue around in while loop so that the wxResolvePathDialog
					// is shown again.
				}
				break;
			case wxResolvePathDialog::e_Abort:
				// Throw exception to stop the load completely
				throw fsFileDoesntExistX(i_OrigFilename);
				break;
			}
		}
	}
#else
	// If no GUI and in strict mode, do not allow a scene to load with missing assets.
	if (l_bStrictMode)
	{
		DBG_ERROR( "File not found.  " << i_OrigFilename );

		// Throw exception to stop the load completely
		throw fsFileDoesntExistX(i_OrigFilename);
	}
	else
	{
		DBG_ERROR( "File not found.  " << i_OrigFilename );
	}
#endif
	return false;
}

//------------------------------------------------------------------------
//	If set to true (this is default) the app will NOT allow any scene to 
//	load if an asset is missing.
//------------------------------------------------------------------------
void mainResolvePath::SetStrictMode(const bool i_bStrictMode)
{
	l_bStrictMode = i_bStrictMode;
}

