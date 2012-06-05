/*****************************************************************************
**  cmpsCompassObjectRotate.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectRotate.hpp"
#include "Support/cmps/cmpsCompass.hpp"

#include "Tool/api3d/api3dScale.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Core/ma/maConstants.hpp"
//#include "Core/ma/maFloatRGBA.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include <algorithm>

namespace
{
	enum PartsIndex
	{
		e_XIndex = 0,
		e_YIndex = 1,
		e_ZIndex = 2
	};

	const float l_cfRotateRingRadius	= 10.0f;
	const float l_cfRotateRingThickness = 0.5f; // 0.15f;
	const int	l_cRotateRingSubdivision = 100;
	const float l_cfSphereRadius		= 0.1f;
	const float l_cfMinScale			= 0.5f;

	const maFloatRGBA lc_HighlightColor(1.0f,1.0f,1.0f,1.0f);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectRotate::cmpsCompassObjectRotate(int i_RenderLayer)
:	cmpsCompassObject(i_RenderLayer)
{
	maRotation rot;

	// create x object and line
	cmpsObjectSimple* pLine = Create_Circle(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), l_cfRotateRingRadius );
	Add_Line(pLine);
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pLine->SetOrientation(rot);

	// create y object and line
	pLine = Create_Circle(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), l_cfRotateRingRadius);
	Add_Line(pLine);
	//rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 1, 0));
	//pLine->SetOrientation(rot);

	// create z object and line
	pLine = Create_Circle(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), l_cfRotateRingRadius);
	Add_Line(pLine);
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pLine->SetOrientation(rot);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectRotate::~cmpsCompassObjectRotate()
{
	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
		api3dScene::RemoveObject(*it, this->GetRenderLayer());

	envSTLHelpers::DeleteContainer(m_Lines);
}

//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObjectRotate::SetPosition(const maPoint3d& i_Position)
{
	cmpsCompassObject::SetPosition( i_Position );

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetPosition(GetPosition());
	}
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObjectRotate::SetScale(const maPoint3d& i_Scale)
{
	if ( GetLockScale() )
		return;

	cmpsCompassObject::SetScale( i_Scale );

	// Scale needs to be uniform for this object?
	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetScale(i_Scale);
	}

	//DBG_LOG1( "Scale = %5.2f", GetScale().GetX() );

}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectRotate::SetOrientation( const maRotation& i_Orientation )
{
	cmpsCompassObject::SetOrientation( i_Orientation );

	const maVector3d vX (1,0,0);
	const maVector3d vY (0,1,0);
	const maVector3d vZ (0,0,1);

	maRotation rot;

	rot.SetValue( maVector3d(0, 1, 0), vX);
	rot = i_Orientation * rot;
	m_Lines[e_XIndex]->SetOrientation(rot);

	//rot.SetValue( maVector3d(0, 1, 0), vY);
	rot = i_Orientation; // * rot;
	m_Lines[e_YIndex]->SetOrientation(rot);

	rot.SetValue( maVector3d(0, 1, 0), vZ);
	rot = i_Orientation * rot;
	m_Lines[e_ZIndex]->SetOrientation(rot);

}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompassObjectRotate::SetRenderable(bool i_bRender)
{
	cmpsCompassObject::SetRenderable( i_bRender );

//	DBG_LOG2("cmpsCompassObjectRotate::SetRenderable %d %d", i_bRender, GetRenderable());

	bool render_x = bool(GetRenderable() && (GetParts() & cmpsCompass::e_X) ? true : false);
	bool render_y = bool(GetRenderable() && (GetParts() & cmpsCompass::e_Y) ? true : false);
	bool render_z = bool(GetRenderable() && (GetParts() & cmpsCompass::e_Z) ? true : false);

	//DBG_LOG3( "renderable %d %d %d", render_x, render_y, render_z );

	m_Lines[e_XIndex]->SetRenderable( render_x );
	m_Lines[e_YIndex]->SetRenderable( render_y );
	m_Lines[e_ZIndex]->SetRenderable( render_z );

}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectRotate::HighlightPart( int i_Part )
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
}

//--------------------------------------------------------------------
//	SetActive turns enabled state of the compass on and off.
//--------------------------------------------------------------------
//virtual 
void cmpsCompassObjectRotate::SetActive(bool i_bActive)
{
	cmpsCompassObject::SetActive( i_bActive );

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->DisplayActive(i_bActive);
	}
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompassObject.
//	If it does, the t value is also returned in o_T and the part that
//	was selected is returned in o_Part
//----------------------------------------------------------------------------
bool cmpsCompassObjectRotate::RayPick(	const maPoint3d& i_RayStart,
										const maPoint3d& i_RayEnd,
										float& o_T,
										int& o_Part )
{
	// if we are not being rendered return false
	if (!GetRenderable())
		return false;

	float t = maConstants::c_fLargest;
	bool intersection = false;

	maPoint3d RaySeg = i_RayEnd - i_RayStart;

	// check sphere
	// doing this first causes it to always select sphere first
	// even if one of the axis objects is on top

	//DBG_LOG0("----------------------------------------------------------------------");
	//DBG_LOG6( "rstart(%6.3f,%6.3f,%6.3f) rend(%6.3f,%6.3f,%6.3f)", i_RayStart.GetX(), i_RayStart.GetY, i_RayStart.GetZ(), i_RayEnd.GetX(), i_RayEnd.GetY(), i_RayEnd.GetZ() );
	//DBG_LOG7( "pos(%6.3f,%6.3f,%6.3f) srad(%f) scale(%6.3f,%6.3f,%6.3f) ", GetPosition().GetX(), GetPosition().GetY(), GetPosition().GetZ(), l_fSphereRadius, GetScale().GetX(), GetScale().GetY(), GetScale().GetZ() );
	//DBG_LOG2( "center=%08x    ptr=%x", (GetParts() & e_Center), Get_Center() );
	//DBG_LOG3( "+ray seg  (%6.3f,%6.3f,%6.3f)", RaySeg.GetX(), RaySeg.GetY(), RaySeg.GetZ() );

/*	if ( (GetParts() & e_Center)
		&& Get_Center()
		&& geoRayIntersection::IntersectLineSphere(
						i_RayStart,
						RaySeg,
						GetPosition(),
						l_cfSphereRadius * GetScale().GetX(),
						t )
		)
	{
		//DBG_LOG0( "intersect: center" );

		intersection = true;
		o_Part = e_Center;
		o_T = t;
	}*/

	// check all three axis for intersection

//	maPoint3d arm_x(l_cfRotateRingRadius * GetScale().GetX(),0,0);
//	maPoint3d arm_y(0,l_cfRotateRingRadius * GetScale().GetY(),0);
//	maPoint3d arm_z(0,0,l_cfRotateRingRadius * GetScale().GetZ());
	maPoint3d arm_x(1,0,0);
	maPoint3d arm_y(0,1,0);
	maPoint3d arm_z(0,0,1);

	GetOrientation().RotateVector( arm_x );
	GetOrientation().RotateVector( arm_y );
	GetOrientation().RotateVector( arm_z );

	//DBG_LOG0( "--------------------------------------" );
	//DBG_LOG1( "ring width=%6.3f", l_cfRotateRingThickness );
	//DBG_LOG3( "arm_x(%6.3f,%6.3f,%6.3f)", arm_x.GetX(), arm_x.GetY(), arm_x.GetZ() );
	//DBG_LOG3( "arm_y(%6.3f,%6.3f,%6.3f)", arm_y.GetX(), arm_y.GetY(), arm_y.GetZ() );
	//DBG_LOG3( "arm_z(%6.3f,%6.3f,%6.3f)", arm_z.GetX(), arm_z.GetY(), arm_z.GetZ() );
	//DBG_LOG3( "scale(%6.3f,%6.3f,%6.3f)", GetScale().GetX(), GetScale().GetY(), GetScale().GetZ() );
	//DBG_LOG3( "cntr(%6.3f,%6.3f,%6.3f)", GetPosition().GetX(), GetPosition().GetY(), GetPosition().GetZ() );

	// check X
	if (geoRayIntersection::IntersectLineThickCircle(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_x,
			l_cfRotateRingRadius * GetScale().GetX(),
			api3dScale::Scale(l_cfRotateRingThickness),
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
	if (geoRayIntersection::IntersectLineThickCircle(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_y,
			l_cfRotateRingRadius * GetScale().GetY(),
			api3dScale::Scale(l_cfRotateRingThickness),
			t)
		)
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
	if (geoRayIntersection::IntersectLineThickCircle(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_z,
			l_cfRotateRingRadius * GetScale().GetZ(),
			api3dScale::Scale(l_cfRotateRingThickness),
			t)
		)
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
bool cmpsCompassObjectRotate::MatchPickCode(envType::UInt32 i_PickCode)
{
	if (!GetRenderable())
		return false;

	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		if ((*it)->ContainsPickCode( i_PickCode ) )
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//	Add_Line adds line to list
//----------------------------------------------------------------------------
void cmpsCompassObjectRotate::Add_Line( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT0(i_pObject,"Object is null");
	m_Lines.push_back(i_pObject);
	api3dScene::AddObject( i_pObject, this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Create_Center creates a circle
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectRotate::Create_Circle(const maFloatRGBA& i_Color, float i_fRadius  )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCircle( i_fRadius, 72 );
	cmpsObjectSimple*	pCircle = new cmpsObjectSimple( pFragment, 
		new matMaterial("Phong.fx") );
	pCircle->SetColor( i_Color );
	pCircle->ModifyEmissive( lc_CompassEmissiveNormal );
	pCircle->SetRenderable( true );
	return pCircle;
}

//--------------------------------------------------------------------
//	using the latest compass settings, modify the object's shape.
//--------------------------------------------------------------------
void cmpsCompassObjectRotate::ResizeObject(const maPoint3d& i_CameraPos)
{
	// A rotation will be around the object's pivot position, 
	// so it is necessary for the rotation compass to be centered on that origin.
	// So, if the object's bbox is far from its origin, the rotate compass
	// will be large; but unfortunately, we don't have any choice.
	//
	//maPoint3d origin = this->GetPosition();
	maPoint3d origin = this->GetPivot();

	//	set the center of the compass to the pivot point
	this->SetPosition( origin );

	maAxisBox box	= GetBounds();
	box.Union(origin); 
	// Also negate all points around the origin 
	// and add them to the bbox
	maVector3d pts[8];
	box.GetBoxPoints(pts);
	for (int i=0; i<8; i++)
	{
		pts[i] -= origin;
		pts[i] *= -1.0f;
		pts[i] += origin;
	}
	box.Union(pts, 8);

	//DBG_LOG3( "max (%6.3f,%6.3f,%6.3f)",  box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );
	//DBG_LOG3( "min (%6.3f,%6.3f,%6.3f)",  box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "diff(%6.3f,%6.3f,%6.3f)",  box.GetDiffX(), box.GetDiffY(), box.GetDiffZ() );
	//DBG_LOG3( "cntr(%6.3f,%6.3f,%6.3f)", box.GetCenter().GetX(), box.GetCenter().GetY(), box.GetCenter().GetZ() );

	//	scale the object accordingly
	//float scalex = box.GetDiffX() / GetDefaultSize();
	//float scaley = box.GetDiffY() / GetDefaultSize();
	//float scalez = box.GetDiffZ() / GetDefaultSize();

	//temporary
	//if (scalex == 0.0f) scalex = 1.0f;
	//if (scaley == 0.0f) scaley = 1.0f;
	//if (scalez == 0.0f) scalez = 1.0f;
	//DBG_LOG3( "select compass scale (%6.3f,%6.3f,%6.3f)", scalex, scaley, scalez );

	//SetScale( maVector3d( scalex, scaley, scalez ) );

	// Compute Camera Scale
	float dist = (i_CameraPos - origin).Length();
	const float c_Modifier = 0.15f;
	const float default_size = GetDefaultSize();
	float camera_scale = dist * c_Modifier / default_size; // could be some formula of distance

	// uniform scale
	float scale = box.GetRadius() / default_size;
	if (scale < camera_scale) scale = camera_scale;
	if (scale < l_cfMinScale) scale = l_cfMinScale;
	SetScale( maVector3d(scale, scale, scale) );
}


//----------------------------------------------------------------------------
//	SetBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectRotate::SetBounds( const maAxisBox& i_Bounds,
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
float cmpsCompassObjectRotate::GetDefaultSize() const
{
	return l_cfRotateRingRadius;
}


