/*****************************************************************************
**	ovrlyObject.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "FCSupport/ovrly/ovrlyObject.hpp"

#include "FCSupport/fcui/fcuiConstants.hpp"
#include "MainApp/mnmApp.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Graphics/eff/effTexturedData.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gpx/gpxSceneObject.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	enum ovrlyStateIndices
	{
		e_None = -1,
		e_Normal = 0,
		e_Highlight,
		e_Disabled,
		e_States
	};
}


//--------------------------------------------------------------------
//	Constructor
//--------------------------------------------------------------------
ovrlyObject::ovrlyObject(itString& i_FileNormal,
						itString& i_FileHighlight,
						itString& i_FileDisable	)
:	m_Width(1.0f),
	m_Height(1.0f),
	m_CurrentState( e_Normal ),
	m_bIsPickable(false),
	m_Position(0.0f, 0.0f)
{
	m_States.resize( e_States );
	create_object(i_FileDisable, &m_States[e_Disabled]);
	create_object(i_FileHighlight, &m_States[e_Highlight]);
	create_object(i_FileNormal, &m_States[e_Normal]);		// set normal last since height + width get set (in case no highlight or disabled)
}

//--------------------------------------------------------------------
//	Destructor
//--------------------------------------------------------------------
ovrlyObject::~ovrlyObject()
{
	for (int i=0; i < m_States.size(); ++i)
	{
		if ( m_States[i].m_pObject != NULL )
		{
			delete m_States[i].m_pObjectProxy;
			api3dScene::RemoveObject(m_States[i].m_pObject);
			delete m_States[i].m_pObject;

			matTextureMgr::ReleaseTexture(m_States[i].m_pTexture);
			m_States[i].m_pTexture = NULL;
		}
	}
}

//--------------------------------------------------------------------
///	Set/Get the position
//--------------------------------------------------------------------
void ovrlyObject::SetPosition( maPoint2d& i_Position )
{
	m_Position = i_Position;

	for (int i=0; i < m_States.size(); ++i)
	{
		m_States[i].m_pObjectProxy->SetPosition(maPoint3d(i_Position.GetX(), i_Position.GetY(), 0));
	}
}
maPoint2d ovrlyObject::GetPosition()
{
	return m_Position;
}

//--------------------------------------------------------------------
///	Set the color
//--------------------------------------------------------------------
void ovrlyObject::SetColor( maFloatRGBA i_Color )
{
	for (int i=0; i < m_States.size(); ++i)
	{
		m_States[i].m_pObjectProxy->SetColor(i_Color);
	}

	//m_States[e_Normal].m_pObject->SetColor( i_Color );
	//effTexturedData* pData = dynamic_cast<effTexturedData*>(pTexMat->GetEffectData());
	//DBG_ASSERT(pData != NULL, "cptrModeRender not using effTextureData");
	//const float fALPHA = 0.5f;
	//const maFloatRGBA objectcolor( 1.0f, 1.0f, 1.0f, fALPHA );
	//pData->m_Color = objectcolor;
}

//--------------------------------------------------------------------
//	Show() - Render the shape or not
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::Show(bool i_bVisible)
{
	if ( m_States[m_CurrentState].m_pObjectProxy != NULL )
	{
		m_States[m_CurrentState].m_pObjectProxy->SetRenderable(i_bVisible);
	}
}

//--------------------------------------------------------------------
///	Inputs are the mouse movement
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::DoMovement( int i_X, int i_Y, int i_Z )
{
}

//--------------------------------------------------------------------
///	This overlay object normal state
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::State_Normal()
{
	m_CurrentState = e_Normal;
	show_state( m_CurrentState );
}

//--------------------------------------------------------------------
///	This overlay object has been picked, do what it needs to do.
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::State_Picked()
{
	m_CurrentState = e_Highlight;
	show_state( m_CurrentState );
}

//--------------------------------------------------------------------
///	This overlay object has been rolled over, do what it needs to do.
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::State_Highlight()
{
	m_CurrentState = e_Highlight;
	show_state( m_CurrentState );
}

//--------------------------------------------------------------------
///	This overlay object has been disabled.
//--------------------------------------------------------------------
//virtual 
void ovrlyObject::State_Disabled()
{
	m_CurrentState = e_Disabled;
	show_state( m_CurrentState );
}

//--------------------------------------------------------------------
///	return true if the screen point is within the bounding area of
///	this object.  If the object is not pickable this will also return
///	false.
//--------------------------------------------------------------------
//virtual 
bool ovrlyObject::CheckForPick( maPoint2d& i_ScreenPoint )
{
	if (m_bIsPickable)
	{
		//	check if the point is within the bounds of this object
		//
		//float m_Width;
		//float m_Height;
		maPoint2d position;
		position = tma3dScreenUtil::ScreenToWindowPosition( m_Position );
		//DBG_TRACE("Check at (" << i_ScreenPoint << ")  obj at (" << position << ")  " << this->m_States[0].m_pTexture->GetFileName() );
		//DBG_TRACE("    width " << m_Width << "   height " << m_Height);

		maPoint2d ul, lr;
		//ul.Set( position.GetX() - (m_Width/2.0f), position.GetY() - (m_Height/2.0f));
		//lr.Set( position.GetX() + (m_Width/2.0f), position.GetY() + (m_Height/2.0f));
		ul.Set( position.GetX(), position.GetY() - m_Height);
		lr.Set( position.GetX() + m_Width, position.GetY());
		//DBG_TRACE("    bounds UL (" << ul << ")   LR (" << lr << ")   [" << m_Width << "x" << m_Height << "]");

		//	if the screen point is within this object, return true.
		if (   (ul.GetX() <= i_ScreenPoint.GetX())
			&& (lr.GetX() >= i_ScreenPoint.GetX())
			&& (ul.GetY() <= i_ScreenPoint.GetY())
			&& (lr.GetY() >= i_ScreenPoint.GetY()))
		{
			DBG_TRACE("    Pick!    bounds UL (" << ul << ")   LR (" << lr << ")   [" << m_Width << "x" << m_Height << "]" << "Check at (" << i_ScreenPoint << ")  obj at (" << position << ")  ");
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool ovrlyObject::IsVisible()
{
	if ( m_States[m_CurrentState].m_pObjectProxy != NULL)
	{
		return m_States[m_CurrentState].m_pObjectProxy->GetRenderable();
	}
	return false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool ovrlyObject::IsPickable()
{
	return m_bIsPickable;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ovrlyObject::Resize()
{
	// stop any running threads
	gpxRenderControl::ConfirmSingleThread();

	//
	//float targetX, targetY;
	//calculate_object_size( targetX, targetY );

	////DBG_LOG("        Target =" << targetX << "," << targetY );
	//float hxs = targetX * 0.5f;
	//float hys = targetY * 0.5f;
	//float hzs = 0.0f * 0.5f;

	////	fill up the matrix and update
	//maPoint3d rect_verts[8];
	//rect_verts[0].Set(-hxs, -hys, +hzs);
	//rect_verts[1].Set(+hxs, -hys, +hzs);
	//rect_verts[2].Set(+hxs, -hys, -hzs);
	//rect_verts[3].Set(-hxs, -hys, -hzs);
	//rect_verts[4].Set(-hxs, +hys, +hzs);
	//rect_verts[5].Set(+hxs, +hys, +hzs);
	//rect_verts[6].Set(+hxs, +hys, -hzs);
	//rect_verts[7].Set(-hxs, +hys, -hzs);
	//m_pObject->Fragment()->UpdateVertices(8, &(rect_verts[0]));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ovrlyObject::create_object(itString& i_FileName, ovrlyState* o_pState)
{
	DBG_ASSERT( o_pState != NULL, "No state" );

	//	if the object exists, free it up first
	//
	if ( o_pState->m_pObjectProxy != NULL )
	{
		//bRender = m_pObject->GetRenderable();
		delete o_pState->m_pObjectProxy;
		api3dScene::RemoveObject( o_pState->m_pObject );
		delete o_pState->m_pObject;
	}

	//
	maPoint2d pos(0,0);
	maPoint3d pos3(0,0,0);
	maPoint2d size(0.25,0.25);

	const bool bMorphable = true;

	fsLocator image = gfPaths::GetPath( mnmPaths::e_AppGUI );
	image.Push(fcuiConstants::c_UI_GUI);
	image.Push(i_FileName);

#if 0
	o_pState->m_pObject = api3dShape::CreateTexturedRectangle(image, objectcolor,
													target_size.GetX(), target_size.GetY(), 1, 1);
#else
	const bool l_MIPMAP = false;
	o_pState->m_pTexture = matTextureMgr::LoadTexture( image, TEXTURE_TYPE_2D, l_MIPMAP );

	maPoint2d targetpt;
	calculate_screen_dimensions( (o_pState->m_pTexture->GetWidth()), (o_pState->m_pTexture->GetHeight()), targetpt );
	m_Width = (float)o_pState->m_pTexture->GetWidth();
	m_Height = (float)o_pState->m_pTexture->GetHeight();

	const bool lc_morphable = false;
	g3dFragment* pFragment;
	pFragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(targetpt.GetX(), targetpt.GetY(), 1, 1, lc_morphable);
	pFragment->SetDoubleSided(true);
	
	// Material
	matMaterial * pTexMat = new matMaterial("Billboard.fx");
	effTexturedData* pData = dynamic_cast<effTexturedData*>(pTexMat->GetEffectData());
	DBG_ASSERT(pData != NULL, "cptrModeRender not using effTextureData");
	const float fALPHA = 0.5f;
	const maFloatRGBA objectcolor( 1.0f, 1.0f, 1.0f, fALPHA );
	pData->m_Color = objectcolor;
	pData->m_Texture = o_pState->m_pTexture;
	pTexMat->SetHasSpecular( false );

	// Set up fragment in object
	o_pState->m_pObject = new api3dObjectSimple(pFragment, pTexMat);
#endif
	o_pState->m_pObject->SetGPUPickable(false);
	o_pState->m_pObject->SetPosition(maPoint3d(0,0,0)); // center of the screen
	o_pState->m_pObject->SetRenderable( false );
	api3dScene::AddObject( o_pState->m_pObject, mnmApp::GetScreenSpaceIndex() );

	// create thread-safe proxy
	o_pState->m_pObjectProxy = new gpxSceneObject(*o_pState->m_pObject);
}

//--------------------------------------------------------------------
///	Calculate the size of the object based on the view size and
///	horizontal and vertical percentages.
//--------------------------------------------------------------------
void ovrlyObject::calculate_object_size(float& o_TargetX, float& o_TargetY)
{
	o_TargetX = 128;
	o_TargetY = 128;

	//maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
	//m_Width = winsize.GetX() * (1.0f - (0.02f * m_HorizontalPercent));	// % -> 0.0 to 1.0 then double
	//m_Height = winsize.GetY() * (1.0f - (0.02f * m_VerticalPercent));	// % -> 0.0 to 1.0 then double
	//maPoint2d origpt( m_Width, m_Height );
	//maPoint2d targetpt;
	//targetpt = tma3dScreenUtil::WindowToScreenPosition( origpt );

	//float targetX = targetpt.GetX();
	//float targetY = targetpt.GetY();
	////DBG_LOG("    TARGETPT  win=" << origpt << "  orig target=" << targetpt);
	////DBG_LOG("    TARGETPT  w,h=" << m_Width << "," << m_Height);
	////DBG_LOG("    TARGETPT %w,h=" << m_HorizontalPercent << "," << m_VerticalPercent);

	//float tx = (1.0f - fabs(targetpt.GetX()));
	//o_TargetX = (targetX) * 2.0f + tx;
	//float ty = (1.0f - fabs(targetpt.GetY()));
	//o_TargetY = (targetY) * 2.0f - ty;
	////DBG_LOG("    TARGETPT  tx,ty=" << o_TargetX << "," << o_TargetY);
}

//--------------------------------------------------------------------
///	Calculate the size of the object assuming the center of the 
///	image is (0,0)
//--------------------------------------------------------------------
void ovrlyObject::calculate_screen_dimensions(int i_TextureWidth, int i_TextureHeight, maPoint2d& o_Target)
{
	float wmw, wmh;		// image dimensions
	wmw = (float)i_TextureWidth;
	wmh = (float)i_TextureHeight;
	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();
	maPoint2d origpt( (float)wmw, (float)wmh );
	o_Target.SetX( origpt.GetX() * (winsize.GetY()/winsize.GetX()) * (2.0f / winsize.GetX()) );
	o_Target.SetY( origpt.GetY() * (2.0f / winsize.GetY()) );

	DBG_TRACE(" win size = " << winsize << "  texture size = " << origpt << "  target=" << o_Target);

	//maPoint2d position;
	//position = tma3dScreenUtil::WindowToScreenPosition( origpt );
	//DBG_TRACE("2win size = " << winsize << "  texture size = " << origpt << "  target=" << position);
}

//--------------------------------------------------------------------
///	Turn off all states, and keep just one
//--------------------------------------------------------------------
void ovrlyObject::show_state(int i_VisibleState)
{
	for (int i=0; i < m_States.size(); ++i)
	{
		if ( m_States[i].m_pObjectProxy != NULL )
		{
			m_States[i].m_pObjectProxy->SetRenderable((i == i_VisibleState));
		}
	}
}

