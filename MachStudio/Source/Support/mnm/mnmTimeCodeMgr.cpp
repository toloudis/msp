/*****************************************************************************
**  mnmTimeCodeMgr.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmTimeCodeMgr.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"

// mach studio
#include "MainApp/mnmApp.hpp"
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

//	library
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTimeUtils.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/scr/scrCreator.hpp"
#include "Graphics/scr/scrText.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gpx/gpxText.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"

#include <sstream>
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace TCMData
{
	scrText*	l_pTimeCodeText = NULL;
	gpxText*	l_pTimeCodeTextProxy = NULL;
	bool		l_bShowTimeCode = false;
	int			l_ScreenSpaceLayer;
	float		l_fFramePerSecond;

	bool		l_bShowSceneAndCamName = true;
	std::string	l_SceneName;
	std::string	l_CamName;
}

namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void format_time_text()
	{
		maTime simtime = tmlnTimeLine::GetValue();
		std::string time_string, frames_string;
		tmlnTimeUtil::GetTimeString(simtime, time_string);
		tmlnTimeUtil::GetTimeStringInFrames(simtime, frames_string);

		//	display the text
		//char text[128];
		//const char* text;
		std::string text;
		if ( TCMData::l_bShowSceneAndCamName )
		{
			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			TCMData::l_CamName = theName.GetString();

			//sprintf( text, "%s (%s) %s-(%s)", time_string.c_str(), frames_string.c_str(), TCMData::l_SceneName.c_str(), TCMData::l_CamName.c_str() );
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss <<time_string<<"("<<frames_string<<")"<<" "<<TCMData::l_SceneName<<"-("<<TCMData::l_CamName<<")";
			text = oss.str();
		}
		else
		{
			//sprintf( text, "%s (%s) %s-(%s)", time_string.c_str(), frames_string.c_str());
			std::ostringstream oss;
			oss.setf(0, std::ios::floatfield);
			oss <<time_string<<"("<<frames_string<<")";
			text = oss.str();
		
		}
		TCMData::l_pTimeCodeTextProxy->SetText( itString( text.c_str() ) );
	}
}

//------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------
void mnmTimeCodeMgr::Initialize()
{
	fsLocator imageloc;
	imageloc.Clear();
	imageloc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) );
	//imageloc.Push( "font-lucd00.png" );
	imageloc.Push( mnmConstants::c_ViewerFontName );

	TCMData::l_pTimeCodeText = scrCreator::MakeText( imageloc, mnmTimeCodeUtil::GetWindow(), 320, 256 );
	TCMData::l_pTimeCodeText->SetForegroundColor( maFloatRGBA(1,1,1,0) );
	TCMData::l_pTimeCodeText->SetBackgroundColor( maFloatRGBA(0,0,0,1) );
	TCMData::l_pTimeCodeText->SetPosition( maPoint3d( 10, 10, 0 ) );
	TCMData::l_pTimeCodeText->SetCentered( false );
	TCMData::l_pTimeCodeText->SetSize( 10, 16 );
	TCMData::l_pTimeCodeText->SetText( itString("00:00:00") );
	TCMData::l_fFramePerSecond = g3dConstants::c_fDefaultFrameRate;

	TCMData::l_ScreenSpaceLayer = mnmApp::GetScreenSpaceIndex();//2;		// FIX: - hard-coded value

	docSingleDocumentMgr::GetFilenameOnly( TCMData::l_SceneName );

	// Create thread-safe proxy
	TCMData::l_pTimeCodeTextProxy  = new gpxText(*TCMData::l_pTimeCodeText);
}

//------------------------------------------------------------------------
// DeInitialize
//------------------------------------------------------------------------
void mnmTimeCodeMgr::DeInitialize()
{
	SetShowTimeCode( false );

	delete TCMData::l_pTimeCodeText;
	TCMData::l_pTimeCodeText = 0;
	delete TCMData::l_pTimeCodeTextProxy;
	TCMData::l_pTimeCodeTextProxy = 0;
}

//------------------------------------------------------------------------
//	Update()
//------------------------------------------------------------------------
void mnmTimeCodeMgr::Update()
{
	if ( !TCMData::l_bShowTimeCode ) return;

	format_time_text();
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void mnmTimeCodeMgr::SetShowTimeCode( bool i_bShow )
{
	//bga - this should probably just be a "SetRenderable" call so that
	// we don´t have to stop the threads.
	if ( i_bShow )
	{
		if ( !TCMData::l_bShowTimeCode )
		{
			// stop running threads before changing scene structure
			gpxRenderControl::ConfirmSingleThread();

			api3dScene::GetRoot( TCMData::l_ScreenSpaceLayer )->AddChild( TCMData::l_pTimeCodeText->GetSceneNode() );
		}
	}
	else
	{
		if ( TCMData::l_bShowTimeCode )
		{
			
			// stop running threads before changing scene structure
			gpxRenderControl::ConfirmSingleThread();

			api3dScene::GetRoot( TCMData::l_ScreenSpaceLayer )->RemoveChild( TCMData::l_pTimeCodeText->GetSceneNode() );
		}
	}

	TCMData::l_bShowTimeCode = i_bShow;
}
bool mnmTimeCodeMgr::IsShowTimeCode()
{
	return TCMData::l_bShowTimeCode;
}


//------------------------------------------------------------------------
//	In frames per second
//------------------------------------------------------------------------
void mnmTimeCodeMgr::SetFrameRate( float i_fFramesPerSecond )
{
	TCMData::l_fFramePerSecond = i_fFramesPerSecond;
}
