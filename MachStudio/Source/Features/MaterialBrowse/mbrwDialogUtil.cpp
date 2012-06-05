/*****************************************************************************
**	mbrwDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"
#include "Features/MaterialBrowse/wxGUI/mbrwMaterialSwatchDialog.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


namespace mbrwDialogUtil
{
	namespace
	{
		fsLocator l_MaterialHomeDir;

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
#ifdef USE_WXWIDGETS
		// Create dialog
		if (!mbrwMaterialSwatchDialog::Instance)
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			mbrwMaterialSwatchDialog::Instance = new mbrwMaterialSwatchDialog(twxSystem::g_pMainForm);
		}
#endif // USE_WXWIDGETS
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
	}

	//--------------------------------------------------------------------
	// ShowMaterialBrowseDialog
	//--------------------------------------------------------------------
	void  ShowMaterialBrowseDialog()
	{
#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(mbrwMaterialSwatchDialog::Instance);
#endif // USE_WXWIDGETS
	}

	//--------------------------------------------------------------------
	// Set home directory for material browsing
	//--------------------------------------------------------------------
	void SetInitialDirectory(const fsLocator& i_MaterialHomeDir)
	{
		l_MaterialHomeDir = i_MaterialHomeDir;
	}
	fsLocator GetInitialDirectory()
	{
		if ((l_MaterialHomeDir.GetNumNames() > 0) &&
			(fsFileUtil::DirectoryExists(l_MaterialHomeDir)))
			return l_MaterialHomeDir;
		else
			return gfPaths::GetPath(gfPaths::e_MaterialLibrary); // Set from user documents folder in mnmPaths
	}

	//--------------------------------------------------------------------
	// Refresh Icons from the current directory, called when
	// new materials are exported.
	//--------------------------------------------------------------------
	void RefreshIcons()
	{
#ifdef USE_WXWIDGETS
		if (mbrwMaterialSwatchDialog::Instance)
		{
			mbrwMaterialSwatchDialog::Instance->RefreshIcons();
		}		
#endif // USE_WXWIDGETS
	}

}	// end of namespace
