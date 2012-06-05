/*****************************************************************************
**  cptrPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/orthoPackage.hpp"

#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Capture/orthoModeRender.hpp"
#include "Features/Capture/orthoModeRenderBatch.hpp"
#include "Features/Capture/orthoModeRenderQuick.hpp"
#include "Features/Capture/orthoModeRenderFrame.hpp"
#include "Features/Capture/orthoModeRenderProgressiveFrame.hpp"

#include "Features/Capture/cptrCommands.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"

#include "Core/Dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"


//============================================================================
//============================================================================
namespace cptrPackage
{
	//========================================================================
	//========================================================================
	namespace
	{	
		orthoModeRender*		l_pModeRender		= NULL;
		cptrModeRenderBatch*	l_pModeRenderBatch	= NULL;
		cptrModeRenderQuick*	l_pModeRenderQuick	= NULL;
		cptrModeRenderFrame*	l_pModeRenderFrame	= NULL;
		cptrModeRenderProgressiveFrame*	l_pModeRenderProgressiveFrame	= NULL;

		modeModeID l_ModeIDCapture;
		modeModeID l_ModeIDBatch;
		modeModeID l_ModeIDQuick;
		modeModeID l_ModeIDFrame;
		modeModeID l_ModeIDProgressiveFrame;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(g2dSystem *i_pSystem)
	{
		// Create the modes
		l_pModeRenderBatch = new cptrModeRenderBatch( l_ModeIDCapture );
		l_pModeRenderQuick = new cptrModeRenderQuick( l_ModeIDCapture );
		l_pModeRenderQuick->SetSystem(i_pSystem);
		l_pModeRenderFrame = new cptrModeRenderFrame( l_ModeIDCapture );
		l_pModeRenderProgressiveFrame = new cptrModeRenderProgressiveFrame( l_ModeIDCapture );
		l_pModeRender = new orthoModeRender();	//	 set last to ensure prefs filename is correct.
		l_pModeRender->SetSystem(i_pSystem);

		//	add to the modeModeMgr
		l_ModeIDCapture = modeModeMgr::AddMode( l_pModeRender, true, "mode-render.png" );
		l_ModeIDQuick = modeModeMgr::AddMode( l_pModeRenderQuick, true, "mode-renderquick.png" );
		l_ModeIDFrame = modeModeMgr::AddMode( l_pModeRenderFrame, true, "mode-renderframe.png" );
		l_ModeIDProgressiveFrame = modeModeMgr::AddMode( l_pModeRenderProgressiveFrame, true, "mode-renderframe.png" );
		l_ModeIDBatch = modeModeMgr::AddMode( l_pModeRenderBatch, true, "mode-renderbatch.png" );

		//	progress dialog settings
		cptrRenderProgressDialogUtil::SetRenderBatchID( l_ModeIDBatch );

		//	set-up the menus + commands
		cptrCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	render data
		//	
		cptrRenderOutputDataUtil::CleanUp();

		// Let modeModeMgr destroy the modes?
		//
		modeModeMgr::RemoveMode(l_pModeRenderBatch);
		delete l_pModeRenderBatch;
		modeModeMgr::RemoveMode(l_pModeRenderQuick);
		delete l_pModeRenderQuick;
		modeModeMgr::RemoveMode(l_pModeRenderFrame);
		delete l_pModeRenderFrame;
		modeModeMgr::RemoveMode(l_pModeRenderProgressiveFrame);
		delete l_pModeRenderProgressiveFrame;
		modeModeMgr::RemoveMode(l_pModeRender);
		delete l_pModeRender;
	}

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (batch)
	//
	//	batch file format: one scene name per line
	//--------------------------------------------------------------------
	void LaunchBatchRender(fsLocator& i_BatchFile)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDBatch );
		DBG_ASSERT0( pMode != 0, "No batch capture mode" );
		cptrModeRenderBatch *pCapMode = dynamic_cast<cptrModeRenderBatch*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "capture mode ID is not the batch capture mode" );

		//	open the batch file and loop through the entries
		//
		try
		{
			cptrRenderBatchData& data = cptrRenderBatchDataUtil::Data();
			cptrRenderBatchDataUtil::ReadData( i_BatchFile, data );
			//DBG_LOG1( "Read %d items", m_Data.m_Scenes.size() );
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsFileDoesntExistX: %s", filename.c_str());
			std::string msg = "File does not exist: " + filename;
			DBG_ERROR1("%s", msg.c_str());
			//MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
		}
		catch( const fsDirectoryDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
			DBG_LOG1("fsDirectoryDoesntExistX: %s", filename.c_str());
			std::string msg = "Directory does not exist: " + filename;
			DBG_ERROR1("%s", msg.c_str());
			//MessageBox::Show(gcnew System::String(msg.c_str()), "Error");
		}
		catch( ... )
		{
			DBG_ERROR0("General exception error");
			//MessageBox::Show("General exception error", "Error");
			throw;
		}

		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//	read in the render preferences
		//	NOTE: is this needed here for batch?
		//
		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderOutput.cfg");
		cptrRenderOutputData data;

		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );

		cptrRenderOutputDataUtil::Data() = data;
		cptrRenderOutputDataUtil::SetSceneAndDirectory(dir);

		//	set-up the render mode to launch directly
		//
		pCapMode->SetSkipCaptureOptions( true );

		if (!modeModeMgr::IsEmpty())
		{
			modeModeMgr::Clear();
		}

		modeModeMgr::Push(l_ModeIDBatch);
	}

	//--------------------------------------------------------------------
	//	Launch the render with the currently loaded scene
	//--------------------------------------------------------------------
	void LaunchRender(int width, int height)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDCapture );
		DBG_ASSERT0( pMode != 0, "No capture mode" );
		orthoModeRender *pCapMode = dynamic_cast<orthoModeRender*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "capture mode ID is not the capture mode" );

		//
		//guiSingleDocHandler::Open(i_SceneFile);
		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//	read in the render preferences
		//
		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderOutput.cfg");

		cptrRenderOutputData data;
		
		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );
		cptrRenderOutputDataUtil::Data() = data;

		if ((width != 0) && (height != 0)) {
			cptrRenderOutputDataUtil::SetWidth(width) ;
			cptrRenderOutputDataUtil::SetHeight(height) ;
		}

		fsLocator sceneFile( docSingleDocumentMgr::GetFilename() );
		cptrRenderOutputDataUtil::SetSceneAndDirectory( sceneFile );

		//	set-up the render mode to launch directly
		//
		pCapMode->SetSkipCaptureOptions( true );

		//if (!modeModeMgr::IsEmpty())
		//{
		//	modeModeMgr::Clear();
		//}

		modeModeMgr::Push(l_ModeIDCapture);
	}

	//--------------------------------------------------------------------
	//	Launch the frame render with the currently loaded scene
	//--------------------------------------------------------------------
	void LaunchProgressiveFrameRender(int startWidth, int startHeight, int width, int height)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDProgressiveFrame );
		DBG_ASSERT0( pMode != 0, "No frame capture mode" );
		cptrModeRenderProgressiveFrame *pCapMode = dynamic_cast<cptrModeRenderProgressiveFrame*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "frame mode ID is not the frame capture mode" );

		//
		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//	read in the render preferences
		//
		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderOutput.cfg");

		cptrRenderOutputData data;

		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );
		cptrRenderOutputDataUtil::Data() = data;

		cptrRenderOutputDataUtil::SetWidth(width) ;
		cptrRenderOutputDataUtil::SetHeight(height) ;

		cptrRenderOutputDataUtil::SetProgressive(true) ;
		cptrRenderOutputDataUtil::SetProgressiveCount(3) ;		// steps
		cptrRenderOutputDataUtil::SetProgressiveStartWidth(startWidth) ;
		cptrRenderOutputDataUtil::SetProgressiveStartHeight(startHeight) ;

		fsLocator sceneFile( docSingleDocumentMgr::GetFilename() );
		cptrRenderOutputDataUtil::SetSceneAndDirectory( sceneFile );

		//	set-up the render mode to launch directly
		//
		//pCapMode->SetSkipCaptureOptions( true );

		modeModeMgr::Push( l_ModeIDProgressiveFrame );
	}


	//--------------------------------------------------------------------
	//	Launch the frame render with the currently loaded scene
	//--------------------------------------------------------------------
	void LaunchFrameRender(int width, int height)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDFrame );
		DBG_ASSERT0( pMode != 0, "No frame capture mode" );
		cptrModeRenderFrame *pCapMode = dynamic_cast<cptrModeRenderFrame*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "frame mode ID is not the frame capture mode" );

		//
		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//	read in the render preferences
		//
		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderOutput.cfg");

		cptrRenderOutputData data;

		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );
		cptrRenderOutputDataUtil::Data() = data;

		if ((width != 0) && (height != 0)) 
		{
			cptrRenderOutputDataUtil::SetWidth(width) ;
			cptrRenderOutputDataUtil::SetHeight(height) ;
		}

		fsLocator sceneFile( docSingleDocumentMgr::GetFilename() );
		cptrRenderOutputDataUtil::SetSceneAndDirectory( sceneFile );

		//	set-up the render mode to launch directly
		//
		//pCapMode->SetSkipCaptureOptions( true );

		modeModeMgr::Push( l_ModeIDFrame );
	}


	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene)
	//--------------------------------------------------------------------
	void LaunchRender(fsLocator& i_SceneFile, int width, int height)
	{
		// now launch the correct mode
		//
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDCapture );
		DBG_ASSERT0( pMode != 0, "No capture mode" );
		orthoModeRender *pCapMode = dynamic_cast<orthoModeRender*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "capture mode ID is not the capture mode" );

		//
		guiSingleDocHandler::Open(i_SceneFile);
		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		//	read in the render preferences
		//
		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderOutput.cfg");
		cptrRenderOutputData data;

		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );

		cptrRenderOutputDataUtil::Data() = data;
		cptrRenderOutputDataUtil::SetSceneAndDirectory( i_SceneFile );

		if ((width != 0) && (height != 0)) {
			cptrRenderOutputDataUtil::SetWidth(width) ;
			cptrRenderOutputDataUtil::SetHeight(height) ;
		}

		//	set-up the render mode to launch directly
		//
		pCapMode->SetSkipCaptureOptions( true );

		if (!modeModeMgr::IsEmpty())
		{
			modeModeMgr::Clear();
		}

		modeModeMgr::Push(l_ModeIDCapture);
	}

	//--------------------------------------------------------------------
	//	Launch the capture for the current file (scene or batch)
	//--------------------------------------------------------------------
	void LaunchQuickRender(fsLocator& i_SceneFile, int width, int height)
	{
		bool bBatch = false;

		// TODO: - only does a single scene file, not a batch file
		guiSingleDocHandler::Open(i_SceneFile);
		mnmAutoSaveMgr::Initialize(false,10,1);	// turn off auto-save

		// now launch the correct mode
		modeMode* pMode = modeModeMgr::GetMode( l_ModeIDCapture );
		DBG_ASSERT0( pMode != 0, "No capture mode" );
		cptrModeRenderQuick *pCapMode = dynamic_cast<cptrModeRenderQuick*>(pMode);
		DBG_ASSERT0( pCapMode != 0, "capture mode ID is not the capture mode" );

		fsLocator dir( gfPaths::GetPath( mnmPaths::e_Configs ) );
		dir.Push("RenderQuickOutput.cfg");
		cptrRenderOutputData data;

		cptrRenderOutputDataUtil::ReadData(dir, data);
		cptrRenderOutputDataUtil::BuildCameraList( data );

		cptrRenderOutputDataUtil::Data() = data;
		cptrRenderOutputDataUtil::SetSceneAndDirectory(dir);

		if ((width != 0) && (height != 0)) {
			cptrRenderOutputDataUtil::SetWidth(width) ;
			cptrRenderOutputDataUtil::SetHeight(height) ;
		}

		pCapMode->SetSkipCaptureOptions( true );

		if (!modeModeMgr::IsEmpty())
		{
			modeModeMgr::Clear();
		}

		if ( !bBatch )
		{
			modeModeMgr::Push(l_ModeIDCapture);
		}
		else
		{
			modeModeMgr::Push(l_ModeIDBatch);
		}
	}

}	// end of namespace
