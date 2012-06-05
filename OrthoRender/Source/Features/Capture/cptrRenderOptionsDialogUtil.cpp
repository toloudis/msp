/*****************************************************************************
**	cptrRenderOptionsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderOptionsDialogUtil.hpp"

#include "Features/Capture/mGUI/cptrRenderOptionsForm.h"
#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsDirectorsCutMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"

#include "Tool/doc/docSingleDocumentMgr.hpp"


//============================================================================
//============================================================================
namespace cptrRenderOptionsDialogUtil
{
	bool l_bExitting;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show( cptrRenderOutputData& o_Data, bool i_bBatchMode )
	{
		cptrRenderOutputDataUtil::SetupControls( i_bBatchMode );
		o_Data.m_bBatchMode.SetValue( i_bBatchMode );

		//	set the marker in/out time so it can be used
		// FIX [rjk] can we remove this reference to chnl somehow?
		//chnlDialogUtil::UpdateData();

		o_Data.m_fMarkerInTime.SetValue( chnlMarkerMgr::GetMarkerInTime() );
		o_Data.m_fMarkerOutTime.SetValue( chnlMarkerMgr::GetMarkerOutTime() );
		//DBG_LOG2("Marker In (%6.3f) Out (%6.3f)", o_Data.m_fMarkerInTime.GetValue(), o_Data.m_fMarkerOutTime.GetValue() );

		cptrRenderOutputDataUtil::BuildCameraList( o_Data );

		//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		//DBG_LOG0("SHOW");
		//int num_cameras = data.m_Cameras.GetNumberOfItems();
		//for (int i = 0; i < num_cameras; ++i)
		//{
		//	int b1, b2;
		//	b1 = data.m_CameraList[i].m_bCapture.GetValue();
		//	b2 = data.m_Cameras.GetValueFlag(i);
		//	DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");
		//}

		cptrRenderOutputDataUtil::UpdateCameraList( o_Data );

		//	config dialog + hide certain components if in batch mode
		cptrRenderUtil::SetCapture( false );
		l_bExitting = false;

#ifdef _MANAGED
		//cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		//data.m_bBatchMode = i_bBatchMode;
		StudioFramework::RenderOptions^ pDialog = gcnew StudioFramework::RenderOptions();

		pDialog->Show();
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();

		//	set the current scene and camera names
		//
		if ( docSingleDocumentMgr::GetFilename().GetNumNames() > 0 )
		{
			itString itPrefix = docSingleDocumentMgr::GetFilename().GetLastName();
			itPrefix.StripExtension();
			cptrRenderOutputDataUtil::SetCurrentScene(itPrefix);
		}

		nameString theName;
		camsFollowUtil::GetCurrentCameraName( theName );
		cptrRenderOutputDataUtil::SetCurrentCamera(theName);

		// sync the cameras
		//
		//DBG_LOG0("HIDE");
		int num_cameras = data.m_Cameras.GetNumberOfItems();
		for (int i = 0; i < num_cameras; ++i)
		{
		//	int b1, b2;
		//	b1 = data.m_CameraList[i].m_bCapture.GetValue();
		//	b2 = data.m_Cameras.GetValueFlag(i);
		//	DBG_LOG3("%02d %s vs %s", i, b1?"true":"false", b2?"true":"false");
			data.m_CameraList[i].m_bCapture.SetValue( data.m_Cameras.GetValueFlag(i) );
		}

		cptrRenderOutputDataUtil::WriteData(data);

		cptrRenderUtil::SetCapture( true );
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
	}
}

