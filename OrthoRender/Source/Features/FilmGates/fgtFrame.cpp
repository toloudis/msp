/*****************************************************************************
**	fgtFrame.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Features/FilmGates/fgtFrame.hpp"

#include "MainApp/mnmApp.hpp"
#include "Tool/api3d/api3dObjectSimple.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
fgtFrame::fgtFrame(int i_Width, int i_Height, char* i_MenuName)
:	m_Width(i_Width),
	m_Height(i_Height),
	m_MenuDesc(i_MenuName),
	m_pFrameObject(NULL)
{
	if ( m_pFrameObject == NULL )
	{
		CreateFrame();
	}
}

//--------------------------------------------------------------------
//	Destructor
//--------------------------------------------------------------------
fgtFrame::~fgtFrame()
{
	if ( m_pFrameObject )
	{
		api3dScene::RemoveObject(m_pFrameObject);
		delete m_pFrameObject;
	}
}

//--------------------------------------------------------------------
//	Show() - Render the shape or not
//--------------------------------------------------------------------
//virtual 
void fgtFrame::Show(bool i_bVisible)
{
	if ( m_pFrameObject )
	{
		m_pFrameObject->SetRenderable(i_bVisible);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool fgtFrame::IsVisible()
{
	if ( m_pFrameObject )
	{
		return m_pFrameObject->GetRenderable();
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgtFrame::Resize()
{
	CreateFrame();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgtFrame::CreateFrame()
{
	bool bRender = false;

	if ( m_pFrameObject )
	{
		bRender = m_pFrameObject->GetRenderable();
		api3dScene::RemoveObject( m_pFrameObject );
		delete m_pFrameObject;
	}

	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
	//DBG_LOG2( "renderwindow(%6.3f,%6.3f)", winsize.GetX(), winsize.GetY() );

	maPoint2d origpt( (float)m_Width, (float)m_Height );
	maPoint2d targetpt;
	targetpt = tma3dScreenUtil::WindowToScreenPosition( origpt );

	float targetX = targetpt.GetX();
	float targetY = targetpt.GetY();
	//DBG_LOG4( "   win(%6.3f,%6.3f) orig target (%6.3f,%6.3f)", origpt.GetX(), origpt.GetY(), targetX, targetY );

	float tempX	= (1.0f - fabs(targetpt.GetX()));
	targetX = (targetX) * 2.0f + tempX;
	float tempY	= (1.0f - fabs(targetpt.GetY()));
	targetY = (targetY) * 2.0f - tempY;
	//DBG_LOG4( "                    new target  (%6.3f,%6.3f)   temp (%6.3f,%6.3f)", targetX, targetY, tempX, tempY );

	m_pFrameObject = api3dShape::CreateLineBlock(maFloatRGBA(1,1,1,1),
												 targetX,
												 targetY,
												 0.0f);
	//dynamic_cast<api3dObjectSimple*>(m_pFrameObject)->SetColor(maFloatRGBA(1,1,1,1));
	m_pFrameObject->SetPosition(maPoint3d(0.0f,0.0f,0.0f));
	m_pFrameObject->SetRenderable(bRender);

	api3dScene::AddObject( m_pFrameObject, mnmApp::GetScreenSpaceIndex() );
}
