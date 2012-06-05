/*****************************************************************************
**  cptrModeRenderBatch.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrModeRenderBatch.hpp"

#include "Features/Capture/cptrRenderBatchData.hpp"
#include "Features/Capture/cptrRenderBatchDataUtil.hpp"
#include "Features/Capture/cptrRenderBatchDialogUtil.hpp"
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/cptrRenderProgressDialogUtil.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrPostRenderUtil.hpp"
#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mode/modeModeMgr.hpp"
//#include "Systems/Particles/Object/prtclObjectMgr.hpp"

//	library
#include "Core/app/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
//#include "Core/it/itStringUtil.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Drivers/Attach/tmlnDriverAttachUtil.hpp"
#include "Support/vis/visMgr.hpp"



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRenderBatch::cptrModeRenderBatch( modeModeID i_ModeIDCapture )
:	m_bInitialized( false ),
	m_bAborted( false ),
	m_bProcessing( false ),
	m_bSkipCaptureOptions( false ),
	m_ModeRenderID( i_ModeIDCapture ),
	m_Stage( e_Begin )
{
	SetMenuItemName( "Batch Render" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
cptrModeRenderBatch::~cptrModeRenderBatch()
{
}

//----------------------------------------------------------------------------
//	Initialize will be called before the first call of Think after
//	the object is first created or DeInitialized.  During the
//	lifetime of a mode, Initialize and DeInitialize may be called
//	several times.  Children of appMode should remember to call
//	appMode::Initialize() at the beginning of their Initialize
//	function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderBatch::Initialize()
{
	if ( !m_bInitialized )
	{
		// get current mode id in order to return to it when finished
	}

	m_bInitialized = true;

	if ( m_bProcessing )
	{
		//	if we are in the middle of processing the batch then jump straight
		//	to processing and load the next scene for capture.
		//
		m_Stage = e_Processing;
	}
	else
	{
		if (m_bSkipCaptureOptions)
		{
			m_Stage = e_SetupForBatch;
		}
		else
		{
			m_Stage = e_Begin;
		}
	}

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Batch Render" );
	visMgr::ShowIcons(false);
	visMgr::ConfirmGeometryVisible();
	tmlnDriverAttachUtil::SetAllowDialogs(false); // should be more general way to do this
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderBatch::DeInitialize()
{
	//if ( m_bInitialized )
	//{
	//}

	m_bInitialized = false;
	m_bAborted = false;
}


//----------------------------------------------------------------------------
//	The mode should do it's per frame "work" in the Think function.
//----------------------------------------------------------------------------
// virtual
void cptrModeRenderBatch::Think()
{
	if ( m_bAborted )
	{
		m_Stage = e_EndProcessing;
	}

	modeMode::Think();

	switch ( m_Stage )
	{
		case e_Begin:
		{
			m_Stage = e_DialogBatch;
			cptrRenderBatchDialogUtil::Show();
			break;
		}
		case e_DialogBatch:
		{
			if ( cptrRenderBatchDialogUtil::IsDialogClosing() )
			{
				m_Stage = e_SetupForBatch;
			}
			else if ( cptrRenderBatchDialogUtil::IsDialogExitting() )	// exit via "X" button
			{
				//	quit
				m_Stage = e_EndMode;
			}
			break;
		}
		case e_SetupForBatch:
		{
			cptrRenderBatchData& theData = cptrRenderBatchDataUtil::Data();
			m_NumScenes = theData.m_Scenes.size();
			m_SceneIndex = 0;
			
			cptrRenderOutputData& capData = cptrRenderOutputDataUtil::Data();
			capData.m_bUseSceneFilenameInFilename = true;
			capData.m_bBatchMode = true;
			capData.m_NumberOfScenes = m_NumScenes;
			capData.m_CurrentSceneNumber = 0;	// gets incremented at start of each render
			//cptrRenderOutputDataUtil::UpdateData( capData );

			if (m_bSkipCaptureOptions)
			{
				m_bProcessing = true;
				cptrRenderUtil::SetCapture(true);
				m_Stage = e_Processing;
			}
			else
			{
				const bool l_bBatchMode = true;
				cptrRenderOptionsDialogUtil::Show( capData, l_bBatchMode );

				m_Stage = e_DialogCapture;
			}

			//	stats: start the statistics data
			//
			std::string fname;
			fsFileUtil::LocatorToANSIFilename( theData.m_BatchFilename.GetValue(), fname );
			DBG_LOG1("mode Batch (%s)", fname.c_str());
			fname = "batch";	// FIX [rjk] get the batch filename to go here.
			cptrRenderStatsDataUtil::StartBatch( fname );

			capData.m_OutputFiles.ClearList();
			break;
		}
		case e_DialogCapture:
		{
			if ( cptrRenderUtil::GetCapture() )
			{
				// the capture dialog is closing so go to processing
				m_Stage = e_Processing;
				m_bProcessing = true;
			}
			break;
		}
		case e_Processing:
		{
			if ( m_SceneIndex < m_NumScenes )
			{
				//DBG_LOG2( "Capturing Scene #%d of %d", m_SceneIndex+1, m_NumScenes );

				cptrRenderBatchData& theData		= cptrRenderBatchDataUtil::Data();
				cptrRenderOutputData& theCapData	= cptrRenderOutputDataUtil::Data();
				theCapData.m_bCaptureAllCameras = true;
				//cptrRenderOutputDataUtil::UpdateData( theCapData );

				if ( theData.m_Scenes[ m_SceneIndex ].m_bChecked.GetValue() )
				{
					//	clear out the particles before loading the new doc
					//
					// TODO: is there a better place to put this clear()? 
					//	this causes modes to have to know about system particles
					//
//					prtclObjectMgr::Clear();

					// load
					guiSingleDocHandler::Open( theData.m_Scenes[m_SceneIndex].m_Filename.GetValue() );

					//if ( !theCapData.m_bAutoGenerateDirectory )
					//{
					//	gfPaths::SetPath( mnmPaths::e_SaveFootage, theCapData.m_Directory );
					//}

					//	set the prefix to be the scene name
					itString current_scenename;
					current_scenename = theData.m_Scenes[m_SceneIndex].m_Filename.GetValue().GetLastName();
					current_scenename.StripExtension();
					cptrRenderOutputDataUtil::SetCurrentScene( current_scenename );

					//	set the max capture frames
					cptrRenderOutputDataUtil::SetMaxTime( 0 );
					theCapData.m_fMarkerInTime.SetValue( chnlMarkerMgr::GetMarkerInTime() );
					theCapData.m_fMarkerOutTime.SetValue( chnlMarkerMgr::GetMarkerOutTime() );

					// process
					cptrModeRender* pModeRender = dynamic_cast<cptrModeRender*>(modeModeMgr::GetMode( m_ModeRenderID ));
					DBG_ASSERT0( pModeRender != 0, "cannot find the capture mode or it's index has changed" );

					pModeRender->SetSkipCaptureOptions( true );
					pModeRender->SetRenderStartTime(appTime::GetTime());
					
					modeModeMgr::Push( this->m_ModeRenderID );

					//	up the scene number
					int snum = (theCapData.m_CurrentSceneNumber.GetValue() + 1);
					theCapData.m_CurrentSceneNumber.SetValue( snum );

					// Capture Director's Cut version if available
					//camsFollowUtil::SetDirectorsCut();

					//DBG_LOG1("Scene Index %d", m_SceneIndex );
				}

				// next scene
				m_SceneIndex++;
			}
			else
			{
				m_Stage = e_EndProcessing;
			}
			break;
		}
		case e_EndProcessing:
		{
			//	stats: end the scene
			cptrRenderStatsDataUtil::EndBatch();

			//	execute the post capture stuff
			//
			cptrPostRenderUtil::Execute();

			cptrRenderOutputData& theCapData = cptrRenderOutputDataUtil::Data();
			if (theCapData.m_RenderPosSaveFile.GetValue().GetNumNames() > 0)
			{
				cptrRenderProgressDialogUtil::DeleteRenderPosition();
			}

			m_Stage = e_EndMode;
			break;
		}
		case e_EndMode:
		{
			//this->SetTerminateCondition(appMode::e_TerminateAndRemove);
			modeModeMgr::Pop();

			// If there is balance between Push and Pop, then there would be
			// no need to Push ObjectManip back on.
			//modeModeMgr::Push( this->m_ModeReturnID );
			//mnpModeObjectManip* pModeRender = dynamic_cast<mnpModeObjectManip*>(modeModeMgr::GetMode( m_ModeObjectManipID ));

			m_bProcessing = false;

			//DBG_LOG0( "End of batch processing" );
			break;
		}
	}
}

//----------------------------------------------------------------------------
//	IsModal() - signals whether the mode should be pushed onto the mode stack
//	ON TOP of the current mode, or replace the current mode (via Pop)
//
//	true - the mode will be pushed on top of the current mode.
//	false- the mode will replace the current mode.
//----------------------------------------------------------------------------
//virtual
bool cptrModeRenderBatch::IsModal()
{
	return true;
}

//--------------------------------------------------------------------
//	Abort all the renders
//--------------------------------------------------------------------
void cptrModeRenderBatch::AbortRenders()
{
	m_bAborted = true;
}

//------------------------------------------------------------------------
//	SetSkipCaptureOptions - if this is set, this mode won't call the
//	capture options dialog.  it will assume that it has already been
//	called by something else.
//
//	Note:  this value will reset with each call to this mode.
//------------------------------------------------------------------------
void cptrModeRenderBatch::SetSkipCaptureOptions( bool i_bSkip )
{
	m_bSkipCaptureOptions = i_bSkip;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRenderBatch::update_scenes_percentage()
{
	char buffer[64];
	sprintf(buffer, "Scene %d of %d", m_SceneIndex+1, m_NumScenes);
	std::string cameras_label(buffer);

	float cameras_percentage;
	cameras_percentage = (float)m_SceneIndex / (float)m_NumScenes;
	cptrRenderProgressDialogUtil::SetCamerasRenderPercentage( cameras_percentage, cameras_label );
}


