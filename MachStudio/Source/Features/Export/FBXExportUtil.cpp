/*****************************************************************************
**	FBXExportUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/Export/FBXExportUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/fs/fsFileStream.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"

#include "Support/cmpr/cmprCompressUtil.hpp"
#include "ImportExport/fbx/export/fbxExportData.hpp"
#include "Support/fbx/fbxMgr.hpp"

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
namespace FBXExportUtil
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
		pCmd = new cmaCommandSimple("To .FBX (all)", 
									"Export", 
									"Package the current scene resources into an fbx file",									
									&FBXExportUtil::NonSelectedExport );
		menu_id = guiMenuMgr::AddMenuItem( "Export", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Export", "PackageScene", pCmd );

		pCmd = new cmaCommandSimple("To .FBX (selected)", 
									"Export", 
									"Package the current scene resources into an fbx file",									
									&FBXExportUtil::SelectedExport );
		menu_id = guiMenuMgr::AddMenuItem( "Export", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Export", "PackageScene", pCmd );
	}

	//------------------------------------------------------------------------
	// NonSelectedExport()
	//------------------------------------------------------------------------
	void NonSelectedExport()
	{
		fbxMgr::SetExportSelected(false);
		BeginExport();
	}

	//------------------------------------------------------------------------
	// SelectedExport()
	//------------------------------------------------------------------------
	void SelectedExport()
	{
		fbxMgr::SetExportSelected(true);
		BeginExport();
	}

	//------------------------------------------------------------------------
	// BeginExport()
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

		docSingleTypeMgr::SetFilter("fbx", "Autodesk FBX Files");
		l_Filter = docSingleTypeMgr::GetFilter();
		l_PackageLocation = docSingleDocumentMgr::GetFilename();
		fsLocator test = docSingleDocumentMgr::GetFilename();
		l_PackageLocation.ReplaceExtension("fbx");
		GetExportFile();		
		docSingleTypeMgr::SetFilter("mab", "Scene Files");
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
				guiSplashScreen::StartUp(splashPath, "Exporting to FBX... please wait.");
			}
			//set wait cursor
			guiCursor::SetWaitCursor();

			//FBXExportUtil will crash when trying to open an existing archive... not sure.
			if(fsFileUtil::FileExists(l_PackageLocation))
				fsFileUtil::DeleteFile(l_PackageLocation);

			//Package Scene
			fbxExportData myExportData = fbxMgr::GetPotentialExportData();

			if ( !myExportData.m_bSceneHasBeenBaked )
				guiMessageBox::Show("Warning: This scene includes materials that have not been baked yet. Such materials may be absent.", 
									"FBX Exporting", guiMessageBox::e_OKOnly);

			fbxMgr::DoExport( l_PackageLocation , myExportData );

			//stop wait cursor
			guiCursor::EndWaitCursor();
			//Close dialog
			guiSplashScreen::ShutDown();
			//reset splash screen to timeout
			guiSplashScreen::SetDoSplashTimeout(true);
		}
	}

} //end FBXExportUtil namespace