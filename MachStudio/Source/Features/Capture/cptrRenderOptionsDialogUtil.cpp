/*****************************************************************************
**	cptrRenderOptionsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"

#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/wxGUI/cptrRenderOptionsDialog.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace cptrRenderOptionsDialogUtil
{
	bool l_bExitting;
	bool l_bVisible = false;

#ifdef USE_WXWIDGETS
	cptrRenderOptionsDialog* l_pRODialog = NULL;
#endif

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Show( captRenderOutputData& o_Data, bool i_bBatchMode )
	{
		//DBG_TRACE("SHOW");
		captRenderOutputDataUtil::SetupControls( i_bBatchMode );
		o_Data.m_bBatchMode.SetValue( i_bBatchMode );

		captRenderOutputDataUtil::BuildCameraList( o_Data );
		captRenderOutputDataUtil::UpdateCameraList( o_Data );

		//	config dialog + hide certain components if in batch mode
		cptrRenderUtil::SetCapture( false );
		l_bExitting = false;

#ifdef USE_WXWIDGETS
		cptrRenderOptionsDialog* pDialog = new cptrRenderOptionsDialog( twxSystem::g_pMainForm );
		if (!l_bVisible)
		{
			pDialog->Show();
			l_bVisible = true;
		}

		l_pRODialog = pDialog;
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void Hide()
	{
		//DBG_TRACE("HIDE");
		captRenderOutputData& data = captRenderOutputDataUtil::Data();

		//	set the current scene and camera names
		//
		if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
		{
			itString itPrefix = docSingleDocumentMgr::GetFilename().GetLastName();
			itPrefix.StripExtension();
			captRenderOutputDataUtil::SetCurrentScene(itPrefix);
		}

		nameString theName;
		camsFollowUtil::GetCurrentCameraName( theName );
		captRenderOutputDataUtil::SetCurrentCamera(theName);

		// sync the cameras
		//
		int num_cameras = data.m_Cameras.GetNumberOfItems();
		if (num_cameras == data.m_CameraList.size())
		{
			for (int i = 0; i < num_cameras; ++i)
			{
				//int b1, b2;
				//b1 = data.m_CameraList[i].m_bCapture.GetValue();
				//b2 = data.m_Cameras.GetValueFlag(i);
				//DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");
				data.m_CameraList[i].m_bCapture.SetValue( data.m_Cameras.GetValueFlag(i) );
			}
		}

		captRenderOutputDataUtil::WriteData(data);

		cptrRenderUtil::SetCapture( true );
#ifdef USE_WXWIDGETS
		l_pRODialog->Hide();
#endif
		l_bVisible = false;
		//l_pRODialog->EndModal(l_bVisible);

	}

	//--------------------------------------------------------------------
	//	returns true if the dialog is exitting by hitting the "X" button
	//--------------------------------------------------------------------
	bool IsExitting()
	{
		return l_bExitting;
	}
	void SetExitting(bool i_bExitting)
	{
		l_bExitting = i_bExitting;
		if (i_bExitting)
			l_bVisible = false;

//		if (i_bExitting)
//			l_pRODialog->EndModal(l_bVisible);

	}
}

