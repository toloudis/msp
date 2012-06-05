/*****************************************************************************
**  cmpsCompassObject3Axis.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/private/cmpsCompassObject3Axis.hpp"

#include "Support/cmps/cmpsCompass.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/icn/icnIconScale.hpp"

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
	float l_fRectSize		= 2.0f;
	float l_fRayPickWidth	= 0.5f;
	float l_fMinScale		= 0.5f;

	const maFloatRGBA lc_HighlightColor(1.0f,1.0f,1.0f,1.0f);

	//------------------------------------------------------------------------
	// Intersects ray with thick cylinder along compass axis.
	// Returns true if the intersect "t" value is closer than the given io_T
	// (which is altered to contain the new lower "t" value).
	//------------------------------------------------------------------------
	bool intersect_arm(const maPoint3d& i_RayStart,
					   const maVector3d& i_RayDir,
					   const maPoint3d& i_Position,
					   const maVector3d& i_Axis,
					   float i_Scale,
					   float& io_T)
	{
		float t = maConstants::c_fLargest;
		if (geoRayIntersection::IntersectLineThickSegment(
				i_RayStart,
				i_RayDir,
				i_Position,
				i_Axis,
				l_fRayPickWidth*i_Scale,
				t)
			)
		{
			if (io_T > t )
			{
				io_T = t;
				return true;
			}
		}
		return false;
	}

	
	//------------------------------------------------------------------------
	// Intersects ray with rectangle in the plane defined by i_Position
	// and i_PlaneNormal with edges defined by i_Edge1 and i_Edge2.
	// Returns true if the intersect "t" value is closer than the given io_T
	// (which is altered to contain the new lower "t" value).
	//------------------------------------------------------------------------
	bool intersect_rect(const maPoint3d& i_RayStart,
					   const maVector3d& i_RayDir,
					   const maPoint3d& i_Position,
					   const maVector3d& i_PlaneNormal,
					   const maVector3d& i_Edge1,
					   const maVector3d& i_Edge2,
					   float& io_T)
	{
		float t = maConstants::c_fLargest;
		const int num_verts = 4;
		maPoint3d rect_verts[num_verts];
		rect_verts[0] = i_Position;
		rect_verts[1] = i_Position + i_Edge1;
		rect_verts[2] = i_Position + i_Edge1 + i_Edge2;
		rect_verts[3] = i_Position + i_Edge2;
		const bool bTwoSided = true;
		if (geoRayIntersection::IntersectLinePolygon(i_RayStart, i_RayDir, 
								rect_verts, num_verts, i_PlaneNormal, 
								t, bTwoSided))
		{
			if (io_T > t )
			{
				io_T = t;
				return true;
			}
		}
		return false;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObject3Axis::cmpsCompassObject3Axis(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompassObject(i_RenderLayer),
	m_pCenter(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObject3Axis::~cmpsCompassObject3Axis()
{

	//std::vector<cmpsObjectSimple*>::iterator it;
	//for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	//	(*it)->RemoveFromScene(this->GetRenderLayer());
	//for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	//	(*it)->RemoveFromScene(this->GetRenderLayer());

	envSTLHelpers::DeleteContainer(m_Objects);
	envSTLHelpers::DeleteContainer(m_Lines);
	envSTLHelpers::DeleteContainer(m_Planes);

	//m_pCenter->RemoveFromScene(this->GetRenderLayer());
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
		m_pCenter->SetPosition( i_Position );
	}

	m_Objects[e_XIndex]->SetPosition(i_Position);
	m_Objects[e_YIndex]->SetPosition(i_Position);
	m_Objects[e_ZIndex]->SetPosition(i_Position);

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetPosition(i_Position);
	}
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
		(*it)->SetPosition(i_Position);
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
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
		(*it)->SetScale(i_Scale);
	}

	if (m_pCenter)
	{
		m_pCenter->SetScale(i_Scale);
	}

	//DBG_LOG1( "Scale = %5.2f", scale.GetX() );
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject3Axis::SetOrientation( const maRotation& i_Orientation )
{
	cmpsCompassObject::SetOrientation( i_Orientation );

	//if we are flipped, we need to adjust the compass coordinates is drawn (?)
	//if(cmpsCompassMgr::GetCompassFlip())

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		(*it)->SetOrientation( i_Orientation );
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetOrientation( i_Orientation );
	}
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
		(*it)->SetOrientation( i_Orientation );
	}

	if ( m_pCenter )
	{
		m_pCenter->SetOrientation( i_Orientation );
	}

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
	bool plane_xy = bool(GetRenderable() && (GetParts() & cmpsCompass::e_PlaneXY) ? true : false);
	bool plane_xz = bool(GetRenderable() && (GetParts() & cmpsCompass::e_PlaneXZ) ? true : false);
	bool plane_yz = bool(GetRenderable() && (GetParts() & cmpsCompass::e_PlaneYZ) ? true : false);

	//DBG_LOG4( "renderable %d %d %d %d", render_x, render_y, render_z, render_center );

	m_Objects[e_XIndex]->SetRenderable( render_x );
	m_Objects[e_YIndex]->SetRenderable( render_y );
	m_Objects[e_ZIndex]->SetRenderable( render_z );

	m_Lines[e_XIndex]->SetRenderable( render_x );
	m_Lines[e_YIndex]->SetRenderable( render_y );
	m_Lines[e_ZIndex]->SetRenderable( render_z );

	if (!m_Planes.empty())
	{
		m_Planes[e_RectXY]->SetRenderable( plane_xy );
		m_Planes[e_RectXZ]->SetRenderable( plane_xz );
		m_Planes[e_RectYZ]->SetRenderable( plane_yz );
	}

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

	if (!m_Planes.empty())
	{
		// set rectangle color using original cmpsCompass indexing method
		m_Planes[e_RectXY]->ModifyColor( ((i_Part==cmpsCompass::e_PlaneXY) ? (lc_HighlightColor) : (m_Planes[e_RectXY]->GetColor())) );
		m_Planes[e_RectXZ]->ModifyColor( ((i_Part==cmpsCompass::e_PlaneXZ) ? (lc_HighlightColor) : (m_Planes[e_RectXZ]->GetColor())) );
		m_Planes[e_RectYZ]->ModifyColor( ((i_Part==cmpsCompass::e_PlaneYZ) ? (lc_HighlightColor) : (m_Planes[e_RectYZ]->GetColor())) );
	}
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
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
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
bool cmpsCompassObject3Axis::RayPick(	int i_IconLayerIndex,
										const maPoint3d& i_RayStart,
										const maPoint3d& i_RayEnd,
										float& o_T,
										int& o_Part )
{
	// if we are not being rendered return false
	if (!GetRenderable())
		return false;

	// Quick check to reject a compass located near the ray start.
	// This happens when a camera being viewed is selected with translate compass.
	if ((i_RayStart - this->GetPosition()).LengthSqr() < maConstants::c_fEpsilon)
		return false;

	float length = l_fLineLength + l_fObjectHeight;
	//maVector3d scale = this->GetScale();
	// get scale for just the icon layer we are picking in
	maVector3d scale = m_Objects[e_XIndex]->GetLayerScale(i_IconLayerIndex);

	maVector3d ray_dir = i_RayEnd - i_RayStart;
	float t = maConstants::c_fLargest;
	bool intersection = false;
	// check sphere
	// doing this first causes it to always select sphere first
	// even if one of the axis objects is on top

	//DBG_LOG("----------------------------------------------------------------------");
	//DBG_LOG6( "rstart(%6.3f,%6.3f,%6.3f) rend(%6.3f,%6.3f,%6.3f)", i_RayStart.GetX(), i_RayStart.GetY, i_RayStart.GetZ(), i_RayEnd.GetX(), i_RayEnd.GetY(), i_RayEnd.GetZ() );
	//DBG_LOG7( "pos(%6.3f,%6.3f,%6.3f) srad(%f) scale(%6.3f,%6.3f,%6.3f) ", GetPosition().GetX(), GetPosition().GetY(), GetPosition().GetZ(), l_fSphereRadius, GetScale().GetX(), GetScale().GetY(), GetScale().GetZ() );
	//DBG_LOG2( "center=%08x    ptr=%x", (GetParts() & e_Center), m_pCenter );

	//bga - Disabling center pick, none of our compasses actually use that pick for anything
	// and it is getting picked incorrectly when in translate mode with scripted camera
	// selected and looked through.
	//
	//if ( (GetParts() & cmpsCompass::e_Center)
	//	&& m_pCenter
	//	&& geoRayIntersection::IntersectLineSphere(
	//					i_RayStart,
	//					ray_dir,
	//					GetPosition(),
	//					l_fSphereRadius * GetScale().GetX(),
	//					t )
	//	)
	//{
	//	//DBG_LOG( "intersect: center" );

	//	intersection = true;
	//	o_Part = cmpsCompass::e_Center;
	//	o_T = t;
	//}

	// check all three axis for intersection
	maPoint3d norm_x(1,0,0);
	GetOrientation().RotateVector( norm_x );
	maPoint3d norm_y(0,1,0);
	GetOrientation().RotateVector( norm_y );
	maPoint3d norm_z(0,0,1);
	GetOrientation().RotateVector( norm_z );
	
	//bga - Note, the flipped state is stored through a negative scale.
	maPoint3d arm_x = norm_x * (length*scale.GetX());
	maPoint3d arm_y = norm_y * (length*scale.GetY());
	maPoint3d arm_z = norm_z * (length*scale.GetZ());
	maPoint3d edge_x = norm_x * (l_fRectSize*scale.GetX());
	maPoint3d edge_y = norm_y * (l_fRectSize*scale.GetY());
	maPoint3d edge_z = norm_z * (l_fRectSize*scale.GetZ());
	
	// check X
	if (intersect_arm(i_RayStart, ray_dir, GetPosition(), arm_x, scale.GetX(), o_T))
	{
		//DBG_LOG( "intersect: x" );
		intersection = true;
		o_Part = cmpsCompass::e_X;
	}

	// check Y
	if (intersect_arm(i_RayStart, ray_dir, GetPosition(), arm_y, scale.GetY(), o_T))
	{
		//DBG_LOG( "intersect: y" );
		intersection = true;
		o_Part = cmpsCompass::e_Y;
	}

	// check Z
	if (intersect_arm(i_RayStart, ray_dir, GetPosition(), arm_z, scale.GetZ(), o_T))
	{
		//DBG_LOG( "intersect: z" );
		intersection = true;
		o_Part = cmpsCompass::e_Z;
	}

	if (!m_Planes.empty())
	{
		// Check XY plane rectangle
		if (intersect_rect(i_RayStart, ray_dir, GetPosition(), norm_z, edge_x, edge_y, o_T))
		{
			//DBG_LOG( "intersect: XY plane" );
			intersection = true;
			o_Part = cmpsCompass::e_PlaneXY;
		}

		// Check XY plane rectangle
		if (intersect_rect(i_RayStart, ray_dir, GetPosition(), norm_y, edge_z, edge_x, o_T))
		{
			//DBG_LOG( "intersect: XZ plane" );
			intersection = true;
			o_Part = cmpsCompass::e_PlaneXZ;
		}

		// Check XY plane rectangle
		if (intersect_rect(i_RayStart, ray_dir, GetPosition(), norm_x, edge_y, edge_z, o_T))
		{
			//DBG_LOG( "intersect: YZ plane" );
			intersection = true;
			o_Part = cmpsCompass::e_PlaneYZ;
		}
	}

	//DBG_LOG( "intersection = " << (intersection ? "true" : "false" ) );
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
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
		if ((*it)->ContainsPickCode( i_PickCode ) )
			return true;
	}

	return false;
}

//--------------------------------------------------------------------
//	SetLayerScale changes the scale of the compass for only a 
// given icon layer (a render panel)
//--------------------------------------------------------------------
void cmpsCompassObject3Axis::SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)
{
	maPoint3d flip_scale = i_Scale;
	if (cmpsCompassMgr::GetCompassFlip())
		flip_scale *= -1;

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Objects.begin(); it != m_Objects.end(); ++it)
	{
		(*it)->SetLayerScale(i_IconLayerIndex, flip_scale);
	}
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetLayerScale(i_IconLayerIndex, flip_scale);
	}
	for ( it = m_Planes.begin(); it != m_Planes.end(); ++it)
	{
		(*it)->SetLayerScale(i_IconLayerIndex, flip_scale);
	}

	if (m_pCenter)
	{
		m_pCenter->SetLayerScale(i_IconLayerIndex, i_Scale);
	}
}

//--------------------------------------------------------------------
//	SetLayerRenderable sets the compass visibility for only a given icon 
// layer (a render panel)
//--------------------------------------------------------------------
void cmpsCompassObject3Axis::SetLayerRenderable(int i_IconLayerIndex, bool i_bRender)
{
	bool render_x = bool(i_bRender && (GetParts() & cmpsCompass::e_X) ? true : false);
	bool render_y = bool(i_bRender && (GetParts() & cmpsCompass::e_Y) ? true : false);
	bool render_z = bool(i_bRender && (GetParts() & cmpsCompass::e_Z) ? true : false);
	bool render_center = bool(i_bRender && (GetParts() & cmpsCompass::e_Center) ? true : false);
	bool plane_xy = bool(i_bRender && (GetParts() & cmpsCompass::e_PlaneXY) ? true : false);
	bool plane_xz = bool(i_bRender && (GetParts() & cmpsCompass::e_PlaneXZ) ? true : false);
	bool plane_yz = bool(i_bRender && (GetParts() & cmpsCompass::e_PlaneYZ) ? true : false);

	//DBG_LOG4( "renderable %d %d %d %d", render_x, render_y, render_z, render_center );

	m_Objects[e_XIndex]->SetLayerRenderable( i_IconLayerIndex, render_x );
	m_Objects[e_YIndex]->SetLayerRenderable( i_IconLayerIndex, render_y );
	m_Objects[e_ZIndex]->SetLayerRenderable( i_IconLayerIndex, render_z );

	m_Lines[e_XIndex]->SetLayerRenderable( i_IconLayerIndex, render_x );
	m_Lines[e_YIndex]->SetLayerRenderable( i_IconLayerIndex, render_y );
	m_Lines[e_ZIndex]->SetLayerRenderable( i_IconLayerIndex, render_z );

	if (!m_Planes.empty())
	{
		m_Planes[e_RectXY]->SetLayerRenderable( i_IconLayerIndex, plane_xy );
		m_Planes[e_RectXZ]->SetLayerRenderable( i_IconLayerIndex, plane_xz );
		m_Planes[e_RectYZ]->SetLayerRenderable( i_IconLayerIndex, plane_yz );
	}

	if ( m_pCenter )
	{
		m_pCenter->SetLayerRenderable( i_IconLayerIndex, render_center );
	}
}


//----------------------------------------------------------------------------
//	Add_Object adds object to list
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Object( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT(i_pObject,"Object is null");
	m_Objects.push_back(i_pObject);
	//i_pObject->AddToScene( this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Add_Line adds line to list
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Line( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT(i_pObject,"Object is null");
	m_Lines.push_back(i_pObject);
	//i_pObject->AddToScene( this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Add_Plane adds plane to list
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Plane( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT(i_pObject,"Object is null");
	m_Planes.push_back(i_pObject);
	//i_pObject->AddToScene( this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Add_Center sets the center object
//----------------------------------------------------------------------------
void cmpsCompassObject3Axis::Add_Center( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT(i_pObject,"Object is null");
	DBG_ASSERT(!m_pCenter,"Center object already exists");
	m_pCenter = i_pObject;
	//i_pObject->AddToScene( this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Create_Line creates a line
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObject3Axis::Create_Line(Axis i_Axis, const maFloatRGBA& i_Color,
													  cmpsRenderLayer::RenderLayer i_RenderLayer)
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

	matMaterial* mat = new matMaterial("Solid.fx");
	g3dFragment* frag = g3dFragmentCreate::CreateLineList( line_vertices,
						 								 line_normals,
														 2,
														 indices,
														 2,
														 mat );
	cmpsObjectSimple* line = new cmpsObjectSimple( frag, mat, i_RenderLayer );
	line->SetColor( i_Color );
	line->ModifyEmissive( lc_CompassEmissiveNormal );
	return line;
}

//----------------------------------------------------------------------------
//	Create_Center creates a sphere
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObject3Axis::Create_Center(const maFloatRGBA& i_Color,
														cmpsRenderLayer::RenderLayer i_RenderLayer)
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateSphere( l_fSphereRadius, 15, 15 );
	cmpsObjectSimple*	pSphere = new cmpsObjectSimple( pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer );
	pSphere->SetColor( i_Color );
	pSphere->ModifyEmissive( lc_CompassEmissiveNormal );
	pSphere->SetRenderable( true );

	return pSphere;
}		

//--------------------------------------------------------------------
//	Create_Plane creates a rectangle for planar movement
//	i_Plane is one of: e_RectXY, e_RectXZ, e_RectYZ 
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObject3Axis::Create_Plane(Plane i_Plane, const maFloatRGBA& i_Color, 
													   cmpsRenderLayer::RenderLayer i_RenderLayer )
{
	maRotation rot;
	// Move the plane out so corner is on the center
	const float plane_shift = l_fRectSize * 0.5f;
	maPoint3d position(plane_shift, plane_shift, plane_shift);

	switch (i_Plane)
	{
	case e_RectXY:
		position.SetZ(0);
		break;
	case e_RectXZ:
		rot.SetValue(maVector3d(1,0,0), maConstants::c_fPI_Div_2);
		position.SetY(0);
		break;
	case e_RectYZ:
		rot.SetValue(maVector3d(0,1,0), maConstants::c_fPI_Div_2);
		position.SetX(0);
		break;
	}

	maMatrix4x4 matx = rot.GetMatrix();
	matx.TranslateBy(position);

	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateLineBlock(l_fRectSize, l_fRectSize, 0);
	cmpsObjectSimple*	pPlane = new cmpsObjectSimple( matx, pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer );
	pPlane->SetColor( i_Color );
	pPlane->ModifyEmissive( lc_CompassEmissiveNormal );
	pPlane->SetRenderable( true );

	return pPlane;
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
//	Resize the compass based on the given camera view
//--------------------------------------------------------------------
void cmpsCompassObject3Axis::ResizeObject(int i_IconLayerIndex, const camCamera& i_Camera )
{
	if (GetLockScale())
		return; 

	// Compute Camera Scale
	maPoint3d pos = this->GetPosition();

	// Simple modifier by distance (only works for perspective cameras)
	//float dist = (i_Camera.GetPosition() - pos).Length();
	//const float c_Modifier = 0.15f;
	//const float default_size = GetDefaultSize();
	//float camera_scale = dist * c_Modifier / default_size;

	// Use icnIconScale's function in order to handle orthographic cameras also
	const float c_Modifier = 3.0f;
	float camera_scale = c_Modifier * icnIconScale::GetIconScaleForPosition(pos, i_Camera);
	camera_scale *= this->GetScalingFactor(); // Comes from Increment/Decrement Compass commands

	if (camera_scale < l_fMinScale) camera_scale = l_fMinScale;
	this->SetLayerScale( i_IconLayerIndex, 
						 maVector3d( camera_scale, camera_scale, camera_scale ) );
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObject3Axis::GetDefaultSize() const
{
	return 1.0f;
}

