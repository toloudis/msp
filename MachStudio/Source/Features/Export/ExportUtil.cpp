/*****************************************************************************
**	ExportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/Export/ExportUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Support/cmpr/cmprCompressUtil.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiSplashScreen.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include <iostream>
#include <string>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace ExportUtil
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	namespace
	{
		std::string l_Filter;
		fsLocator l_PackageLocation;
		std::string l_PackageExt;

	} // end anonymous namspace

	//------------------------------------------------------------------------
	//  AddToMenu() - add export command to menu
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: File->Export->Package Scene
		pCmd = new cmaCommandSimple("To .ZIP", 
									"Export", 
									"Package the current scene resources into a zip file",
									
									&ExportUtil::BeginExport );
		menu_id = guiMenuMgr::AddMenuItem( "Export", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Export", "PackageScene", pCmd );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void BeginExport()
	{
		//check if mab needs to be saved
		if(!guiSingleDocHandler::SaveIfDirty())
			return;

		l_PackageLocation = docSingleDocumentMgr::GetFilename();
		if(!fsFileUtil::FileExists(l_PackageLocation))
		{
			DBG_WARNING("Scene File: " << l_PackageLocation << " no longer exists.");
			guiSingleDocHandler::SaveAs();
			l_PackageLocation = docSingleDocumentMgr::GetFilename();
			
			//if the user still hasn't saved a new mab, return
			if (!fsFileUtil::FileExists(l_PackageLocation))
				return;
		}

		cmprCompressUtil::SetCompressionType(cmprCompressUtil::e_Zip);
		cmprCompressUtil::GetCompressionEXT(l_PackageExt);
		docSingleTypeMgr::SetFilter(l_PackageExt, "Scene Package");
		l_Filter = docSingleTypeMgr::GetFilter();
		
		l_PackageLocation.ReplaceExtension(l_PackageExt.c_str());
		GetExportFile();
		
		docSingleTypeMgr::SetFilter(".mab", "Scene Files");
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void GetExportFile()
	{
		if (guiFileDialogUtils::GetSaveFileName(l_Filter, l_PackageLocation))
		{
			//open a dialog to notify the user that the packaging process is taking place
			guiSplashScreen::SetDoSplashTimeout(false);
			fsLocator splashPath = gfPaths::GetPath(gfPaths::e_ExePath);
			splashPath.Push("Data");
			splashPath.Push("Load.png");
			if (fsFileUtil::FileExists(splashPath))
			{
				guiSplashScreen::StartUp(splashPath, "Packaging to ZIP... please wait.");
			}
			//set wait cursor
			guiCursor::SetWaitCursor();

			//ExportUtil will crash when trying to open an existing archive
			if(fsFileUtil::FileExists(l_PackageLocation))
				fsFileUtil::DeleteFile(l_PackageLocation);

			//Package Scene
			GatherSceneResources();

			//stop wait cursor
			guiCursor::EndWaitCursor();
			//Close dialog
			guiSplashScreen::ShutDown();
			//reset splash screen to timeout
			guiSplashScreen::SetDoSplashTimeout(true);
		}
	}

	//------------------------------------------------------------------------
	// Gathers the scene resources from the resource tracker and sets up the directory
	//------------------------------------------------------------------------
	void GatherSceneResources()
	{
		//fsResourceTrackerData& tracker_data = fsResourceTracker::GetData();
		//bga - switched resource tracking to a document request
		std::set<fsLocator> asset_filenames;
		docSingleDocumentMgr::GetResourceList(asset_filenames);

		fsFileUtil::CreateFile(l_PackageLocation);
		cmprCompressUtil::PrepareCompression();
		
		//first get the mab scene
		fsLocator asset_file = docSingleDocumentMgr::GetFilename();
		if(!PackageScene(asset_file))
			return;

		//now get the rest of the scene's resources
		std::set<fsLocator>::const_iterator it;
		for (it = asset_filenames.begin(); it != asset_filenames.end(); ++it)
		{
			//grab the next asset from the tracker and add it to our locator
			asset_file = (*it);
			
			//prevent empty files in resource list
			if( asset_file.GetNumNames() == 0 )
				continue;

			//add the new file to our zip archive
			if(!PackageScene( asset_file ))
				break;
			
		}
	}

	//------------------------------------------------------------------------
	// Package each resource file and add it to the archive
	//------------------------------------------------------------------------
	bool PackageScene(const fsLocator& i_SourceFile )
	{
		//compress the current file
		bool bSuccess = cmprCompressUtil::CompressFile(i_SourceFile, l_PackageLocation);

		return bSuccess;
	}

} //end ExportUtil namespace