/*****************************************************************************
**  mnmTimeCodeMgr.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmTimeCodeMgr.hpp"

#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"

// mach studio
#include "Support/cams/camsCameraMgr.hpp"
#include "Support/cams/camsFollowUtil.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

//	library
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTimeUtils.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/scr/scrCreator.hpp"
#include "Graphics/scr/scrText.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace TCMData
{
	scrText*	l_pTimeCodeText = 0;
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
		float simtime = tmlnTimeLine::GetValue();
		std::string time_string, frames_string;
		tmlnTimeUtil::GetTimeString(simtime, time_string);
		tmlnTimeUtil::GetTimeStringInFrames(simtime, frames_string);

		//	display the text
		char text[128];
		if ( TCMData::l_bShowSceneAndCamName )
		{
			nameString theName;
			camsFollowUtil::GetCurrentCameraName( theName );
			TCMData::l_CamName = theName.GetString();

			sprintf( text, "%s (%s) %s-(%s)", time_string.c_str(), frames_string.c_str(), TCMData::l_SceneName.c_str(), TCMData::l_CamName.c_str() );
		}
		else
		{
			sprintf( text, "%s (%s) %s-(%s)", time_string.c_str(), frames_string.c_str());
		}

		TCMData::l_pTimeCodeText->SetText( itString( text ) );
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
	imageloc.Push( "font-lucd00.png" );
	TCMData::l_pTimeCodeText = scrCreator::MakeText( imageloc, mnmTimeCodeUtil::GetWindow(), 320, 256 );
	TCMData::l_pTimeCodeText->SetForegroundColor( maFloatRGBA(1,1,1,0) );
	TCMData::l_pTimeCodeText->SetBackgroundColor( maFloatRGBA(0,0,0,1) );
	TCMData::l_pTimeCodeText->SetPosition( maPoint3d( 10, 10, 0 ) );
	TCMData::l_pTimeCodeText->SetCentered( false );
	TCMData::l_pTimeCodeText->SetSize( 10, 16 );
	TCMData::l_pTimeCodeText->SetText( itString("00:00:00") );
	TCMData::l_fFramePerSecond = g3dConstants::c_fDefaultFrameRate;

	TCMData::l_ScreenSpaceLayer = 2;		// FIX: - hard-coded value

	docSingleDocumentMgr::GetFilenameOnly( TCMData::l_SceneName );
}

//------------------------------------------------------------------------
// DeInitialize
//------------------------------------------------------------------------
void mnmTimeCodeMgr::DeInitialize()
{
	SetShowTimeCode( false );

	delete TCMData::l_pTimeCodeText;
	TCMData::l_pTimeCodeText = 0;
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
	if ( i_bShow )
	{
		if ( !TCMData::l_bShowTimeCode )
			api3dScene::GetRoot( TCMData::l_ScreenSpaceLayer )->AddChild( TCMData::l_pTimeCodeText->GetSceneNode() );
	}
	else
	{
		if ( TCMData::l_bShowTimeCode )
			api3dScene::GetRoot( TCMData::l_ScreenSpaceLayer )->RemoveChild( TCMData::l_pTimeCodeText->GetSceneNode() );
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
