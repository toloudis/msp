/*****************************************************************************
**  cptrModeRenderBake.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrModeRenderBake.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrRenderBakeData.hpp"
#include "Features/Capture/cptrRenderBakeDataUtil.hpp"
#include "Features/Capture/cptrRenderBakeDialogUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"


#include "Core/env/envSTLHelpers.hpp"
//#include "Core/name/nameString.hpp"
#include "Support/vis/visMgr.hpp"
//#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

namespace
{
	std::map<int, std::string> l_OutputImageTypeMap;
	std::map<int, int> l_OutputImageResolutionMap;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cptrModeRenderBake::cptrModeRenderBake( modeModeID i_ModeIDCapture )
:	m_bInitialized( false ),
	m_bAborted( false ),
	m_bProcessing( false ),
	m_bSkipCaptureOptions( false ),
	m_ModeRenderID( i_ModeIDCapture ),
	m_Stage( e_Begin )
{
	SetMenuItemName( "Bake Textures" );

	l_OutputImageTypeMap[cptrRenderBakeData::bk_DDS] = std::string("dds");
	l_OutputImageTypeMap[cptrRenderBakeData::bk_BMP] = std::string("bmp");
	l_OutputImageTypeMap[cptrRenderBakeData::bk_JPG] = std::string("jpg");
	l_OutputImageTypeMap[cptrRenderBakeData::bk_PNG] = std::string("png");
	l_OutputImageTypeMap[cptrRenderBakeData::bk_TIF] = std::string("tif");

	l_OutputImageResolutionMap[cptrRenderBakeData::bk_512] = 512;
	l_OutputImageResolutionMap[cptrRenderBakeData::bk_1024] = 1024;
	l_OutputImageResolutionMap[cptrRenderBakeData::bk_2048] = 2048;
	l_OutputImageResolutionMap[cptrRenderBakeData::bk_4096] = 4096;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// virtual
cptrModeRenderBake::~cptrModeRenderBake()
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
void cptrModeRenderBake::Initialize()
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
			m_Stage = e_SetupForBake;
		}
		else
		{
			m_Stage = e_Begin;
		}
	}

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mode, "Bake Textures" );
	//visMgr::ShowIcons(false);
	visMgr::ConfirmGeometryVisible();
	//tmlnDriverAttachUtil::SetAllowDialogs(false); // should be more general way to do this
}

//----------------------------------------------------------------------------
//	The object should clean up things that are not needed while the
//	mode is not running in the DeInitialize function.
//----------------------------------------------------------------------------
//virtual
void cptrModeRenderBake::DeInitialize()
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
void cptrModeRenderBake::Think()
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
			m_Stage = e_DialogBake;
			cptrRenderBakeDialogUtil::Show();
			break;
		}
		case e_DialogBake:
		{
			if ( cptrRenderBakeDialogUtil::IsDialogExitting() )	// exit via "X" button
			{
				//	quit
				m_Stage = e_EndMode;
			}
			else if ( cptrRenderBakeDialogUtil::IsDialogClosing() )
			{
				m_Stage = e_SetupForBake;
			}
			break;
		}
		case e_SetupForBake:
		{
			m_bProcessing = true;

			if (cptrRenderBakeDataUtil::Data().m_bIsSaveAndReplace.GetValue())
			{
				// stop any render threads (needed for saving?)
				gpxRenderControl::ConfirmSingleThread();
				guiCursor::SetWaitCursor();
				guiSingleDocHandler::SaveAs();
				guiCursor::EndWaitCursor();
			}
			
			rlyrRenderLayerMgr::GetObjectsEditorVisibility();
			rlyrRenderLayerMgr::SetupLayerObjectsVisible(cptrRenderBakeDataUtil::GetBakeLayerName());
			cptrRenderBakeDataUtil::ApplyBakeDataOnRenderLayer();
			cptrRenderBakeDataUtil::ApplyBakeDataOnRenderPasses();
			applyBakeRenderPref();

			cptrRenderUtil::BeginCapture();
			
			m_Stage = e_Processing;
			break;
		}
		case e_Processing:
		{
			// call api3d bake next node here
			cptrRenderBakeData& data = cptrRenderBakeDataUtil::Data();
			std::string filename;
			docSingleDocumentMgr::GetFilenameOnly(filename);
			itString sfname(filename.c_str());
			sfname.StripExtension();
			
			fsLocator path = data.m_OutputDir.GetValue();
			path.Push(itStringUtil::GetStdString(sfname).c_str());

			path.Push("BakedTextures");

			if ( !fsFileUtil::DirectoryExists( path ) )
			{
				try
				{
					fsFileUtil::CreateDirectory(path);
				}
				catch ( const fsDirectoryDoesntExistX& /*i_Ex*/ )
				{
					std::string dir;
					fsFileUtil::LocatorToANSIFilename(path, dir);
					std::string msg = "Error trying to create Output directory: " + dir;
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Create Directory Error", guiMessageBox::e_OKOnly);
					m_bAborted = true;
				}
			}

			if (m_bAborted)
			{
				m_Stage = e_EndProcessing;
			}

			// loop over all enabled render passes, if render pass name is in directory.
			nameString layer_desc = cptrRenderBakeDataUtil::GetBakeLayerName();
			int loopPasses = 1;
			std::vector<rlyrPassesObject::ePassType> thePasses;
			rlyrPassesObject* passesObject = rlyrRenderLayerMgr::GetLayerRenderPasses(layer_desc);
			passesObject->CollectPasses(thePasses);
			loopPasses = thePasses.size();

			for (int k = 0; k < loopPasses; k++)
			{
				if (k == 0 && data.m_bIsSaveAndReplace.GetValue())
					chtrObjectMgr::SetBakedFlag(false);

				rlyrPassesObject::ePassType passType = thePasses[k];
				cptrRenderBakeDataUtil::AdjustRenderPref(passType);
				applyBakeRenderPref();

				fgmtOperations::Bake(data.m_bIsSaveAndReplace.GetValue(), path, 
				l_OutputImageTypeMap[data.m_OutputFormat.GetValue()],
				l_OutputImageResolutionMap[data.m_OutputResolution.GetValue()]);

				if (k == loopPasses - 1 && data.m_bIsSaveAndReplace.GetValue())
				{
					chtrObjectMgr::CreateBakedMaterials(path, l_OutputImageTypeMap[data.m_OutputFormat.GetValue()]);
					chtrObjectMgr::GetBakedFlagFromFrags();
					chtrObjectMgr::ReplaceBakedMaterials();
				}
			}

			m_Stage = e_EndProcessing;
			break;
		}
		case e_EndProcessing:
		{
			rlyrRenderLayerMgr::RestoreActiveInRenderLayer();
			cptrRenderUtil::EndCapture();

			if (cptrRenderBakeDataUtil::Data().m_bIsSaveAndReplace.GetValue())
			{
				guiSingleDocHandler::Save();
			}

			m_bAborted = false;
			m_Stage = e_EndMode;
			break;
		}
		case e_EndMode:
		{
			modeModeMgr::Pop();

			m_bSkipCaptureOptions = false;
			m_bProcessing = false;
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
bool cptrModeRenderBake::IsModal()
{
	return true;
}

//--------------------------------------------------------------------
//	Abort all the renders
//--------------------------------------------------------------------
void cptrModeRenderBake::AbortRenders()
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
void cptrModeRenderBake::SetSkipCaptureOptions( bool i_bSkip )
{
	m_bSkipCaptureOptions = i_bSkip;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRenderBake::update_scenes_percentage()
{
	// update progress dialog here
	/*std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << "Scene "<<m_SceneIndex+1<<" of "<<m_NumScenes;
	std::string buffer(oss.str());
	itString cameras_label(buffer.c_str());

	float cameras_percentage;
	cameras_percentage = (float)m_SceneIndex / (float)m_NumScenes;
	cptrRenderProgressDialogUtil::SetCamerasRenderPercentage( cameras_percentage, cameras_label );*/
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cptrModeRenderBake::applyBakeRenderPref()
{
	rlyrRenderLayerMgr::ApplyLayerPrefs(cptrRenderBakeDataUtil::GetBakeLayerName());
}
