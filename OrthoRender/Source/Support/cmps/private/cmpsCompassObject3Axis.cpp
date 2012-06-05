/*****************************************************************************
**  cmpsCompassObject3Axis.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObject3Axis.hpp"

#include "Support/cmps/cmpsCompass.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	enum PartsIndex
	{
		e_XIndex = 0,
		e_YIndex = 1,
		e_ZIndex = 2
	};

	float l_fObjectHeight	= 1.0f;
	//float l_fObjectRadius	= 0.3f;
	float l_fSphereRadius	= 0.1f;
	float l_fLineLength		= 5.0f;
	float l_fRayPickWidth	= 0.5f;
	float l_fMinScale		= 0.5f;

	const maFloatRGBA lc_HighlightColor(1.0f,1.0f,1.0f,1.0f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObject3Axis::cmpsCompassObject3Axis(int i_RenderLayer)
:	cmpsCompassObject(i_RenderLayer),
	m_pCenter(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObject3Axis::~cmpsCompassObject3Axis()
{

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
		api3dScene::RemoveObject(*it, this->GetRenderLayer());
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
		api3dScene::RemoveObject(*it, this->GetRenderLayer());

	envSTLHelpers::DeleteContainer(m_Objects);
	envSTLHelpers::DeleteContainer(m_Lines);

	api3dScene::RemoveObject(m_pCenter, this->GetRenderLayer());
	delete m_pCenter;
	m_pCenter = NULL;
}


//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::SetPosition(const maPoint3d& i_Position)
{
	cmpsCompassObject::SetPosition( i_Position );

	if (m_pCenter)
	{
		m_pCenter->SetPosition( GetPosition() );
	}

	maRotation rot = m_Objects[e_XIndex]->GetOrientation();
	maPoint3d object_position = maPoint3d(0.0f,l_fLineLength*GetScale().GetX(),0.0f);
	rot.RotateVector(object_position);
	m_Objects[e_XIndex]->SetPosition(GetPosition() + object_position);

	rot = m_Objects[e_YIndex]->GetOrientation();
	object_position = maPoint3d(0.0f,l_fLineLength*GetScale().GetY(),0.0f);
	rot.RotateVector(object_position);
	m_Objects[e_YIndex]->SetPosition(GetPosition() + object_position);

	object_position = maPoint3d(0.0f,l_fLineLength*GetScale().GetZ(),0.0f);
	rot = m_Objects[e_ZIndex]->GetOrientation();
	rot.RotateVector(object_position);
	m_Objects[e_ZIndex]->SetPosition(GetPosition() + object_position);

	std::vector<cmpsObjectSimple*>::iterator it;

	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetPosition(GetPosition());
	}
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::SetScale(const maPoint3d& i_Scale)
{
	if ( GetLockScale() )
		return;

	cmpsCompassObject::SetScale( i_Scale );

	std::vector<cmpsObjectSimple*>::iterator it;

	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		(*it)->SetScale(i_Scale);
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetScale(i_Scale);
	}

	if (m_pCenter)
	{
		m_pCenter->SetScale(GetScale());
	}

	//DBG_LOG1( "Scale = %5.2f", GetScale().GetX() );

	// reset the object positions
	SetPosition(GetPosition());
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject3Axis::SetOrientation( const maRotation& i_Orientation )
{
	cmpsCompassObject::SetOrientation( i_Orientation );

	const maVector3d vX (1,0,0);
	const maVector3d vY (0,1,0);
	const maVector3d vZ (0,0,1);

	maRotation rot;

	rot.SetValue( maVector3d(0, 1, 0), vX);
	rot = i_Orientation * rot;
	m_Objects[e_XIndex]->SetOrientation(rot);
	m_Lines[e_XIndex]->SetOrientation(i_Orientation);

	//rot.SetValue( maVector3d(0, 1, 0), vY);
	rot = i_Orientation; // * rot;
	m_Objects[e_YIndex]->SetOrientation(rot);
	m_Lines[e_YIndex]->SetOrientation(i_Orientation);

	rot.SetValue( maVector3d(0, 1, 0), vZ);
	rot = i_Orientation * rot;
	m_Objects[e_ZIndex]->SetOrientation(rot);
	m_Lines[e_ZIndex]->SetOrientation(i_Orientation);

	// reset the object positions
	SetPosition(GetPosition());
}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::SetRenderable(bool i_bRender)
{
	cmpsCompassObject::SetRenderable( i_bRender );

//	DBG_LOG2("cmpsCompassObject3Axis::SetRenderable %d %d", i_bRender, GetRenderable());

	bool render_x = bool(GetRenderable() && (GetParts() & cmpsCompass::e_X) ? true : false);
	bool render_y = bool(GetRenderable() && (GetParts() & cmpsCompass::e_Y) ? true : false);
	bool render_z = bool(GetRenderable() && (GetParts() & cmpsCompass::e_Z) ? true : false);
	bool render_center = bool(GetRenderable() && (GetParts() & cmpsCompass::e_Center) ? true : false);

	//DBG_LOG4( "renderable %d %d %d %d", render_x, render_y, render_z, render_center );

	m_Objects[e_XIndex]->SetRenderable( render_x );
	m_Objects[e_YIndex]->SetRenderable( render_y );
	m_Objects[e_ZIndex]->SetRenderable( render_z );

	m_Lines[e_XIndex]->SetRenderable( render_x );
	m_Lines[e_YIndex]->SetRenderable( render_y );
	m_Lines[e_ZIndex]->SetRenderable( render_z );

	if ( m_pCenter )
	{
		m_pCenter->SetRenderable( render_center );
	}
}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject3Axis::HighlightPart( int i_Part )
{
	//	change from the enum flag to the index
	if ( e_X == i_Part )
		i_Part = e_XIndex;
	else if ( e_Y == i_Part )
		i_Part = e_YIndex;
	else if ( e_Z == i_Part )
		i_Part = e_ZIndex;


	//	Set the color 
	m_Lines[e_XIndex]->ModifyColor( ((i_Part==e_XIndex) ? (lc_HighlightColor) : (m_Lines[e_XIndex]->GetColor())) );
	m_Lines[e_YIndex]->ModifyColor( ((i_Part==e_YIndex) ? (lc_HighlightColor) : (m_Lines[e_YIndex]->GetColor())) );
	m_Lines[e_ZIndex]->ModifyColor( ((i_Part==e_ZIndex) ? (lc_HighlightColor) : (m_Lines[e_ZIndex]->GetColor())) );
	m_Objects[e_XIndex]->ModifyColor( ((i_Part==e_XIndex) ? (lc_HighlightColor) : (m_Objects[e_XIndex]->GetColor())) );
	m_Objects[e_YIndex]->ModifyColor( ((i_Part==e_YIndex) ? (lc_HighlightColor) : (m_Objects[e_YIndex]->GetColor())) );
	m_Objects[e_ZIndex]->ModifyColor( ((i_Part==e_ZIndex) ? (lc_HighlightColor) : (m_Objects[e_ZIndex]->GetColor())) );
}

//--------------------------------------------------------------------
//	SetActive turns enabled state of the compass on and off.
//--------------------------------------------------------------------
//virtual 
void cmpsCompassObject3Axis::SetActive(bool i_bActive)
{
	cmpsCompassObject::SetActive( i_bActive );

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		(*it)->DisplayActive(i_bActive);
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->DisplayActive(i_bActive);
	}
	if (m_pCenter)
	{
		m_pCenter->DisplayActive(i_bActive);
	}
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompassObject.
//	If it does, the t value is also returned in o_T and the part that
//	was selected is returned in o_Part
//----------------------------------------------------------------------------
bool cmpsCompassObject3Axis::RayPick(	const maPoint3d& i_RayStart,
										const maPoint3d& i_RayEnd,
										float& o_T,
										int& o_Part )
{
	// if we are not being rendered return false
	if (!GetRenderable())
		return false;

	float length = l_fLineLength + l_fObjectHeight;

	float t = maConstants::c_fLargest;
	bool intersection = false;

	// check sphere
	// doing this first causes it to always select sphere first
	// even if one of the axis objects is on top

	//DBG_LOG0("----------------------------------------------------------------------");
	//DBG_LOG6( "rstart(%6.3f,%6.3f,%6.3f) rend(%6.3f,%6.3f,%6.3f)", i_RayStart.GetX(), i_RayStart.GetY, i_RayStart.GetZ(), i_RayEnd.GetX(), i_RayEnd.GetY(), i_RayEnd.GetZ() );
	//DBG_LOG7( "pos(%6.3f,%6.3f,%6.3f) srad(%f) scale(%6.3f,%6.3f,%6.3f) ", GetPosition().GetX(), GetPosition().GetY(), GetPosition().GetZ(), l_fSphereRadius, GetScale().GetX(), GetScale().GetY(), GetScale().GetZ() );
	//DBG_LOG2( "center=%08x    ptr=%x", (GetParts() & e_Center), m_pCenter );

	if ( (GetParts() & cmpsCompass::e_Center)
		&& m_pCenter
		&& geoRayIntersection::IntersectLineSphere(
						i_RayStart,
						i_RayEnd - i_RayStart,
						GetPosition(),
						l_fSphereRadius * GetScale().GetX(),
						t )
		)
	{
		//DBG_LOG0( "intersect: center" );

		intersection = true;
		o_Part = cmpsCompass::e_Center;
		o_T = t;
	}

	// check all three axis for intersection

	maPoint3d arm_x(length*GetScale().GetX(),0,0);
	maPoint3d arm_y(0,length*GetScale().GetY(),0);
	maPoint3d arm_z(0,0,length*GetScale().GetZ());

	GetOrientation().RotateVector( arm_x );
	GetOrientation().RotateVector( arm_y );
	GetOrientation().RotateVector( arm_z );

	//DBG_LOG1( "raywidth=%6.3f", l_fRayPickWidth );
	//DBG_LOG3( "arm_x(%6.3f,%6.3f,%6.3f)", arm_x.GetX(), arm_x.GetY(), arm_x.GetZ() );
	//DBG_LOG3( "arm_y(%6.3f,%6.3f,%6.3f)", arm_y.GetX(), arm_y.GetY(), arm_y.GetZ() );
	//DBG_LOG3( "arm_z(%6.3f,%6.3f,%6.3f)", arm_z.GetX(), arm_z.GetY(), arm_z.GetZ() );

	// check X
	if (geoRayIntersection::IntersectLineThickSegment(
			i_RayStart,
			i_RayEnd - i_RayStart,
			GetPosition(),
			arm_x,
			l_fRayPickWidth*GetScale().GetX(),
			t)
		)
	{
		//DBG_LOG0( "intersect: x" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_X;
			o_T = t;
		}
	}

	// check Y
	if (geoRayIntersection::IntersectLineThickSegment(
			i_RayStart, i_RayEnd - i_RayStart, GetPosition(),
			arm_y,
			l_fRayPickWidth*GetScale().GetY(), t))
	{
		//DBG_LOG0( "intersect: y" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_Y;
			o_T = t;
		}
	}

	// check Z
	if (geoRayIntersection::IntersectLineThickSegment(
			i_RayStart, i_RayEnd - i_RayStart, GetPosition(),
			arm_z,
			l_fRayPickWidth*GetScale().GetZ(), t))
	{
		//DBG_LOG0( "intersect: z" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_Z;
			o_T = t;
		}
	}

	//DBG_LOG1( "intersection = %s", (intersection ? "true" : "false" ) );
	return intersection;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmpsCompassObject3Axis::MatchPickCode(envType::UInt32 i_PickCode)
{
	if (!GetRenderable())
		return false;

	if ( (GetParts() & cmpsCompass::e_Center)
		&& m_pCenter)
	{
		if (m_pCenter->ContainsPickCode( i_PickCode ) )
			return true;
	}

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		if ((*it)->ContainsPickCode( i_PickCode ) )
			return true;
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		if ((*it)->ContainsPickCode( i_PickCode ) )
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//	Add_Object adds object to list
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Object( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT0(i_pObject,"Object is null");
	m_Objects.push_back(i_pObject);
	api3dScene::AddObject( i_pObject, this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Add_Line adds line to list
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Line( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT0(i_pObject,"Object is null");
	m_Lines.push_back(i_pObject);
	api3dScene::AddObject( i_pObject, this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Add_Center sets the center object
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Center( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT0(i_pObject,"Object is null");
	DBG_ASSERT0(!m_pCenter,"Center object already exists");
	m_pCenter = i_pObject;
	api3dScene::AddObject( i_pObject, this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Create_Line creates a line
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObject3Axis::Create_Line(Axis i_Axis, const maFloatRGBA& i_Color )
{
	maPoint3d line_vertices[2];
	line_vertices[0].Set( 0, 0, 0 );
	switch (i_Axis)
	{
	case e_AxisX:
		line_vertices[1].Set( l_fLineLength, 0, 0 );
		break;
	default:
	case e_AxisY:
		line_vertices[1].Set( 0, l_fLineLength, 0 );
		break;
	case e_AxisZ:
		line_vertices[1].Set( 0, 0, l_fLineLength );
		break;
	}

	maPoint3d line_normals[2];
	line_normals[0].Set(0,1,0);
	line_normals[1].Set(0,1,0);

	unsigned short indices[2];

	// bottom face
	indices[0] = 0;
	indices[1] = 1;

	matMaterial* mat = new matMaterial("Phong.fx");
	g3dFragment* frag = g3dFragmentCreate::CreateLineList( line_vertices,
						 								 line_normals,
														 2,
														 indices,
														 2,
														 mat );
	cmpsObjectSimple* line = new cmpsObjectSimple( frag, mat );
	line->SetColor( i_Color );
	line->ModifyEmissive( lc_CompassEmissiveNormal );
	return line;
}

//----------------------------------------------------------------------------
//	Create_Center creates a sphere
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObject3Axis::Create_Center(const maFloatRGBA& i_Color )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateSphere( l_fSphereRadius, 15, 15 );
	cmpsObjectSimple*	pSphere = new cmpsObjectSimple( pFragment, 
		new matMaterial("Phong.fx") );
	pSphere->SetColor( i_Color );
	pSphere->ModifyEmissive( lc_CompassEmissiveNormal );
	pSphere->SetRenderable( true );

	return pSphere;
}

//--------------------------------------------------------------------
//	Get_Center returns the center object
//--------------------------------------------------------------------
//virtual
const cmpsObjectSimple* cmpsCompassObject3Axis::Get_Center() const
{
	return m_pCenter;
}

//--------------------------------------------------------------------
//	using the latest compass settings, modify the object's shape.
//--------------------------------------------------------------------
void cmpsCompassObject3Axis::ResizeObject(const maPoint3d& i_CameraPos)
{
	maAxisBox box	= GetBounds();

	//	set the select box to be the center of the bounding area
	//	(ignoring the object's position)
	//
	maPoint3d pos( box.GetCenter() );
	cmpsCompassObject3Axis::SetPosition( pos );

	// Compute Camera Scale
	float dist = (i_CameraPos - pos).Length();
	const float c_Modifier = 0.15f;
	const float default_size = GetDefaultSize();
	float camera_scale = dist * c_Modifier / default_size; // could be some formula of distance

	//DBG_LOG3( "max (%6.3f,%6.3f,%6.3f)",  box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );
	//DBG_LOG3( "min (%6.3f,%6.3f,%6.3f)",  box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "diff(%6.3f,%6.3f,%6.3f)",  box.GetDiffX(), box.GetDiffY(), box.GetDiffZ() );
	//DBG_LOG3( "cntr(%6.3f,%6.3f,%6.3f)", box.GetCenter().GetX(), box.GetCenter().GetY(), box.GetCenter().GetZ() );

	//	scale the object accordingly
	float scalex = box.GetDiffX() / default_size;
	if (scalex < camera_scale) scalex = camera_scale;
	float scaley = box.GetDiffY() / default_size;
	if (scaley < camera_scale) scaley = camera_scale;
	float scalez = box.GetDiffZ() / default_size;
	if (scalez < camera_scale) scalez = camera_scale;

	if (scalex < l_fMinScale) scalex = l_fMinScale;
	if (scaley < l_fMinScale) scaley = l_fMinScale;
	if (scalez < l_fMinScale) scalez = l_fMinScale;
	//DBG_LOG3( "select compass scale (%6.3f,%6.3f,%6.3f)", scalex, scaley, scalez );

	cmpsCompassObject3Axis::SetScale( maVector3d( scalex, scaley, scalez ) );
}


//----------------------------------------------------------------------------
//	SetBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject3Axis::SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos )
{
	// First, let the parent set the actual bounds
	cmpsCompassObject::SetBounds( i_Bounds, i_WorldPivot, i_CameraPos );

	// now we need to scale and position the object based on the bounds.
	//
	ResizeObject(i_CameraPos);
}


//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObject3Axis::GetDefaultSize() const
{
	return 1.0f;
}

