/*****************************************************************************
**	fgtFrame.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FilmGates/fgtFrame.hpp"

#include "MainApp/mnmApp.hpp"

#include "Graphics/g3d/g3dFragment.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
fgtFrame::fgtFrame()
:	m_Width(1.0f),
	m_Height(1.0f),
	m_Ratio(5.0f),
	m_pFrameObject(NULL),
	m_HorizontalPercent(0.0f),
	m_VerticalPercent(0.0f)
{
	create_frame();
}

//--------------------------------------------------------------------
//	Destructor
//--------------------------------------------------------------------
fgtFrame::~fgtFrame()
{
	if ( m_pFrameObject )
	{
		delete m_pFrameObjectProxy;
		api3dScene::RemoveObject(m_pFrameObject);
		delete m_pFrameObject;
	}
}

//--------------------------------------------------------------------
///	Set the horizontal and veritcal percent.
//--------------------------------------------------------------------
void fgtFrame::SetPercentages( float i_HorizontalPct, float i_VerticalPct )
{
	m_HorizontalPercent = i_HorizontalPct;
	m_VerticalPercent = i_VerticalPct;
}


//--------------------------------------------------------------------
///	Set the color
//--------------------------------------------------------------------
void fgtFrame::SetColor( maFloatRGBA i_Color )
{
	m_pFrameObject->SetColor( i_Color );
}

//--------------------------------------------------------------------
//	Show() - Render the shape or not
//--------------------------------------------------------------------
//virtual 
void fgtFrame::Show(bool i_bVisible)
{
	if ( m_pFrameObjectProxy )
	{
		m_pFrameObjectProxy->SetRenderable(i_bVisible);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool fgtFrame::IsVisible()
{
	if ( m_pFrameObjectProxy )
	{
		return m_pFrameObjectProxy->GetRenderable();
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgtFrame::Resize()
{
	// stop any running threads
	gpxRenderControl::ConfirmSingleThread();

	//
	float targetX, targetY;
	calculate_frame_size( targetX, targetY );

	//DBG_LOG("        Target =" << targetX << "," << targetY );
	float hxs = targetX * 0.5f;
	float hys = targetY * 0.5f;
	float hzs = 0.0f * 0.5f;

	//	fill up the matrix and update
	maPoint3d rect_verts[8];
	rect_verts[0].Set(-hxs, -hys, +hzs);
	rect_verts[1].Set(+hxs, -hys, +hzs);
	rect_verts[2].Set(+hxs, -hys, -hzs);
	rect_verts[3].Set(-hxs, -hys, -hzs);
	rect_verts[4].Set(-hxs, +hys, +hzs);
	rect_verts[5].Set(+hxs, +hys, +hzs);
	rect_verts[6].Set(+hxs, +hys, -hzs);
	rect_verts[7].Set(-hxs, +hys, -hzs);
	m_pFrameObject->Fragment()->UpdateVertices(8, &(rect_verts[0]));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void fgtFrame::create_frame()
{
	bool bRender = false;

	if ( m_pFrameObject )
	{
		bRender = m_pFrameObject->GetRenderable();
		delete m_pFrameObjectProxy;
		api3dScene::RemoveObject( m_pFrameObject );
		delete m_pFrameObject;
	}

	//
	float targetX, targetY;
	calculate_frame_size( targetX, targetY );

	const bool bMorphable = true;
	const float fALPHA = 1.0f;
	const maFloatRGBA framecolor( 0.25f, 0.25f, 0.25f, fALPHA );
	m_pFrameObject = api3dShape::CreateLineBlock(framecolor,
												 targetX,
												 targetY,
												 0.0f,
												 bMorphable);
	m_pFrameObject->SetPosition(maPoint3d(0.0f,0.0f,0.0f));
	m_pFrameObject->SetRenderable(bRender);

	api3dScene::AddObject( m_pFrameObject, mnmApp::GetScreenSpaceIndex() );

	// create thread-safe proxy
	m_pFrameObjectProxy = new gpxSceneObject(*m_pFrameObject);
}

//--------------------------------------------------------------------
///	Calculate the size of the frame based on the view size and
///	horizontal and vertical percentages.
//--------------------------------------------------------------------
void fgtFrame::calculate_frame_size(float& o_TargetX, float& o_TargetY)
{
	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
	m_Width = winsize.GetX() * (1.0f - (0.02f * m_HorizontalPercent));	// % -> 0.0 to 1.0 then double
	m_Height = winsize.GetY() * (1.0f - (0.02f * m_VerticalPercent));	// % -> 0.0 to 1.0 then double
	maPoint2d origpt( m_Width, m_Height );
	maPoint2d targetpt;
	targetpt = tma3dScreenUtil::WindowToScreenPosition( origpt );

	float targetX = targetpt.GetX();
	float targetY = targetpt.GetY();
	//DBG_LOG("    TARGETPT  win=" << origpt << "  orig target=" << targetpt);
	//DBG_LOG("    TARGETPT  w,h=" << m_Width << "," << m_Height);
	//DBG_LOG("    TARGETPT %w,h=" << m_HorizontalPercent << "," << m_VerticalPercent);

	float tx = (1.0f - fabs(targetpt.GetX()));
	o_TargetX = (targetX) * 2.0f + tx;
	float ty = (1.0f - fabs(targetpt.GetY()));
	o_TargetY = (targetY) * 2.0f - ty;
	//DBG_LOG("    TARGETPT  tx,ty=" << o_TargetX << "," << o_TargetY);
}

