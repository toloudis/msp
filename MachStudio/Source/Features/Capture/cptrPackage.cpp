/*****************************************************************************
**  cptrPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrPackage.hpp"

#include "Features/Capture/cptrCommands.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBake.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrModeRenderFrame.hpp"
#include "Features/Capture/cptrPythonCommands.hpp"
#include "Features/Capture/cptrRenderBakeDataUtil.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/wxGUI/cptrDialogUtil.hpp"
#include "Features/Capture/wxGUI/cptrToPhotoshopdialog.hpp"
#include "Support/capt/captOutputDataDocumentInterest.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/capt/captRenderProgressMgr.hpp"
#include "Support/capt/captOutputRenderProgressInterest.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmErrorCodes.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#define USE_BAKE_TEXTURE

//============================================================================
//============================================================================
namespace cptrPackage
{
	//========================================================================
	//========================================================================
	namespace
	{	
		cptrModeRender*				l_pModeRender		= NULL;
		cptrModeRenderBatch*		l_pModeRenderBatch	= NULL;
		cptrModeRenderBake*			l_pModeRenderBake	= NULL;
		cptrModeRenderFrame*		l_pModeRenderFrame	= NULL;
		captRenderProgressInterest* l_pRenderPI = NULL;

		modeModeID l_ModeIDCapture;
		modeModeID l_ModeIDBatch;
		modeModeID l_ModeIDBake;
		modeModeID l_ModeIDFrame;
	
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_RenderTagsInfo()
		{
			cptrDialogUtil::ShowTagsInfo();
		}
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem)
	{
		// Create the modes
		l_pModeRenderBatch = new cptrModeRenderBatch( l_ModeIDCapture );
#ifdef USE_BAKE_TEXTURE
		l_pModeRenderBake = new cptrModeRenderBake( l_ModeIDCapture );
#endif

		l_pModeRenderFrame = new cptrModeRenderFrame( l_ModeIDCapture );
		l_pModeRender = new cptrModeRender();	//	 set last to ensure prefs filename is correct.
		l_pModeRender->SetSystem(i_pSystem);

		//	add to the modeModeMgr
		l_ModeIDCapture = modeModeMgr::AddMode( l_pModeRender, true, "mode-render.png" );
		l_ModeIDFrame = modeModeMgr::AddMode( l_pModeRenderFrame, true, "mode-renderframe.png" );
		l_ModeIDBatch = modeModeMgr::AddMode( l_pModeRenderBatch, true, "mode-renderbatch.png" );
#ifdef USE_BAKE_TEXTURE
		l_ModeIDBake = modeModeMgr::AddMode(l_pModeRenderBake, true, "mode-renderbake.png");
#endif

		//	progress dialog settings
		cptrRenderProgressDialogUtil::SetRenderBatchID( l_ModeIDBatch );

		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Render Tags dialog
		pCmd = new cmaCommandSimple("Render Tags List", 
									"Windows", 
									"List of the valid tags for a customized file or directory",
									&Execute_RenderTagsInfo );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Render Tags List", pCmd );

		//add python support
		cptrPythonCommands::AddCommands("mach");

		//	set-up the menus + commands
		cptrCommands::SetupMenu();
		Photoshop::SetupMenu();
		Photoshop::EnableMenus();
		Photoshop::SetFrameID(l_ModeIDFrame);

		docSingleTypeMgr::AddDocumentInterest(new captOutputDataDocumentInterest());
		if(l_pRenderPI == NULL)
		{
			l_pRenderPI = new captOutputRenderProgressInterest();
			captRenderProgressMgr::RegisterInterest(l_pRenderPI);
		}
		cptrRenderBakeDataUtil::Init();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		cptrRenderBakeDataUtil::Cleanup();
		captRenderOutputDataUtil::CleanUp();

		// Let modeModeMgr destroy the modes?
		//
		modeModeMgr::RemoveMode(l_pModeRenderBatch);
		delete l_pModeRenderBatch;
		modeModeMgr::RemoveMode(l_pModeRenderBake);
		delete l_pModeRenderBake;
		modeModeMgr::RemoveMode(l_pModeRenderFrame);
		delete l_pModeRenderFrame;
		modeModeMgr::RemoveMode(l_pModeRender);
		delete l_pModeRender;

		if( l_pRenderPI != NULL)
		{
			captRenderProgressMgr::UnRegisterInterest(l_pRenderPI);
			delete l_pRenderPI;
			l_pRenderPI = NULL;
		}


	}

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (batch)
	//
	//	batch file format: one scene name per line
	//--------------------------------------------------------------------
	void LaunchBatchRender(const fsLocator& i_BatchFile, bool i_bClose)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDBatch );
		DBG_ASSERT( pMode != 0, "No batch capture mode" );
		cptrModeRenderBatch *pCapMode = dynamic_cast<cptrModeRenderBatch*>(pMode);
		DBG_ASSERT( pCapMode != 0, "capture mode ID is not the batch capture mode" );

		//	open the batch file and loop through the entries
		//
		try
		{
			cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
			cptrRenderBatchDataUtil::ReadData( i_BatchFile, data );
			//DBG_LOG( "Read items=" << data.m_Scenes.size() );
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Error reading batch data, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Batch Error", guiMessageBox::e_OKOnly);
			return;
		}

		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//
		//	TODO: send the batch file to cptrModeRenderBatch
		//	TODO: get cptrModeRenderBatch to handle getting a batch file (don't show dialogs, etc)
		//

		//	set-up the render mode to launch directly
		//
		pCapMode->SetSkipCaptureOptions( true );

		if (i_bClose)
		{
			if (!modeModeMgr::IsEmpty())
			{
				modeModeMgr::Clear();
			}
		}
		modeModeMgr::Push(l_ModeIDBatch);
	}

	//--------------------------------------------------------------------
	//	Launch the capture for the current scene (bake texture)
	//--------------------------------------------------------------------
	//void LaunchBakeRender(bool i_bClose)
	//{
	//	// now launch the correct mode
	//	//
	//	modeMode* pMode = modeModeMgr::GetMode( l_ModeIDBake );
	//	DBG_ASSERT( pMode != 0, "No bake capture mode" );
	//	cptrModeRenderBake *pCapMode = dynamic_cast<cptrModeRenderBake*>(pMode);
	//	DBG_ASSERT( pCapMode != 0, "capture mode ID is not the batch capture mode" );

	//	//	open the batch file and loop through the entries
	//	//
	//	try
	//	{
	//		//cptrRenderBakeData& data = cptrRenderBatchDataUtil::Data();
	//		//cptrRenderBakeDataUtil::ReadData( i_BatchFile, data );
	//		//DBG_LOG( "Read items=" << data.m_Scenes.size() );
	//	}
	//	catch ( const envExceptionX& i_Ex )
	//	{
	//		std::string msg = "Error reading bake data, " + i_Ex.GetErrorMessage();
	//		DBG_ERROR(msg);
	//		guiMessageBox::Show(msg.c_str(), "Bake Error", guiMessageBox::e_OKOnly);
	//		return;
	//	}

	//	//mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

	//	//
	//	//	TODO: send the batch file to cptrModeRenderBatch
	//	//	TODO: get cptrModeRenderBatch to handle getting a batch file (don't show dialogs, etc)
	//	//

	//	//	set-up the render mode to launch directly
	//	//
	//	//pCapMode->SetSkipCaptureOptions( true );

	//	if (i_bClose)
	//	{
	//		if (!modeModeMgr::IsEmpty())
	//		{
	//			modeModeMgr::Clear();
	//		}
	//	}
	//	modeModeMgr::Push(l_ModeIDBake);
	//}

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//--------------------------------------------------------------------
	void LaunchRender(const fsLocator& i_SceneFile, bool i_bClearModeStack)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDCapture );
		DBG_ASSERT( pMode != 0, "No capture mode" );
		cptrModeRender *pCapMode = dynamic_cast<cptrModeRender*>(pMode);
		DBG_ASSERT( pCapMode != 0, "capture mode ID is not the capture mode" );

		if (!fsFileUtil::FileExists(i_SceneFile))
		{
			DBG_ERROR("File does not exist: " << i_SceneFile);
			mnmAppUtil::SetErrorCode(mnmErrorCodes::c_SceneFileNotFound); // could not load scene
		}
		else
		{
			//
			guiSingleDocHandler::Open(i_SceneFile);
			
			// If the scene file load fails, the filename will be empty
			if (docSingleDocumentMgr::GetFilename().GetNumNames() == 0)
			{
				// When running with /render then we want the 
				// program to exit when done rendering. If there was an error loading
				// the scene file, then clear the mode list so that it exits right away.
				if (!modeModeMgr::IsEmpty())
				{
					modeModeMgr::Clear();
				}
				mnmAppUtil::SetErrorCode(mnmErrorCodes::c_CouldNotLoadScene); // could not load scene
			}
			else
			{
				mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

				rlyrRenderLayerMgr::Update();
				captRenderOutputData& data = captRenderOutputDataUtil::Data();
				int num_cameras = data.m_Cameras.GetNumberOfItems();

#ifdef DEMO_VERSION
				data.m_nWidth.SetValue( 852 );
				data.m_nHeight.SetValue( 480 );
				data.m_CaptureFormat.SetValue( "JPG" );				// frame or movie format
				data.m_CompressCode.SetValue( "None" );
#endif
				captRenderOutputDataUtil::BuildCameraList( data );

				//older mab scenes may have incorrect camera data after the scene is initially opened
				//so we need ot verify that the sizes of the 2 lists are the same
				if(data.m_CameraList.size() == num_cameras)
				{
					for (int i = 0; i < num_cameras; ++i)
					{
						data.m_CameraList[i].m_bCapture.SetValue( data.m_Cameras.GetValueFlag(i) );
					}
				}

				//captRenderOutputDataUtil::Data() = data;
				//captRenderOutputDataUtil::SetSceneAndDirectory( i_SceneFile );

				//	set the prefix to be the scene name
				itString current_scenename;
				current_scenename = docSingleDocumentMgr::GetFilename().GetLastName();
				current_scenename.StripExtension();
				captRenderOutputDataUtil::SetCurrentScene( current_scenename );

				//	set-up the render mode to launch directly
				//
				pCapMode->SetSkipCaptureOptions( true );

				if (!modeModeMgr::IsEmpty() && i_bClearModeStack)
				{
					modeModeMgr::Clear();
				}

				modeModeMgr::Push(l_ModeIDCapture);
			}
		}
	}

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//--------------------------------------------------------------------
	void LaunchRender()
	{
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDCapture );
		DBG_ASSERT( pMode != 0, "No capture mode" );
		cptrModeRender *pCapMode = dynamic_cast<cptrModeRender*>(pMode);
		DBG_ASSERT( pCapMode != 0, "capture mode ID is not the capture mode" );

		rlyrRenderLayerMgr::Update();
		captRenderOutputData& data = captRenderOutputDataUtil::Data();
		int num_cameras = data.m_Cameras.GetNumberOfItems();
		captRenderOutputDataUtil::BuildCameraList( data );

		//older mab scenes may have incorrect camera data after the scene is initially opened
		//so we need ot verify that the sizes of the 2 lists are the same
		if (data.m_CameraList.size() == num_cameras)
		{
			for (int i = 0; i < num_cameras; ++i)
			{
				data.m_CameraList[i].m_bCapture.SetValue( data.m_Cameras.GetValueFlag(i) );
			}
		}

		//	set the prefix to be the scene name
		itString current_scenename;
		current_scenename = docSingleDocumentMgr::GetFilename().GetLastName();
		current_scenename.StripExtension();
		captRenderOutputDataUtil::SetCurrentScene( current_scenename );

		//	set-up the render mode to launch directly
		//
		pCapMode->SetSkipCaptureOptions( true );

		modeModeMgr::Push(l_ModeIDCapture);
	}

	//--------------------------------------------------------------------
	//	return true if the current mode is one of the render modes
	//--------------------------------------------------------------------
	bool IsRenderModeActive()
	{
		bool bRenderModeActive =(  modeModeMgr::IsCurrentMode(l_ModeIDCapture)
								|| modeModeMgr::IsCurrentMode(l_ModeIDBatch)
								|| modeModeMgr::IsCurrentMode(l_ModeIDBake)
								|| modeModeMgr::IsCurrentMode(l_ModeIDFrame));
		return bRenderModeActive;
	}
	
}	// end of namespace