/*****************************************************************************
**  cmpsCompassObjectRotate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/private/cmpsCompassObjectRotate.hpp"

#include "Support/cmps/cmpsCompass.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
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

	const float l_cfRotateRingRadius	= 10.0f;
	const float l_cfRotateRingThickness = 1.5f; //4.0f; // 0.15f;
	const int	l_cRotateRingSubdivision = 100;
	const float l_cfSphereRadius		= 0.1f;
	const float l_cfMinScale			= 0.5f;

	const maFloatRGBA lc_HighlightColor(1.0f,1.0f,1.0f,1.0f);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectRotate::cmpsCompassObjectRotate(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompassObject(i_RenderLayer)
{
	maRotation rot;

	// create x object and line
	cmpsObjectSimple* pLine = Create_Circle(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), l_cfRotateRingRadius, i_RenderLayer );
	Add_Line(pLine);
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pLine->SetOrientation(rot);

	// create y object and line
	pLine = Create_Circle(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), l_cfRotateRingRadius, i_RenderLayer);
	Add_Line(pLine);
	//rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 1, 0));
	//pLine->SetOrientation(rot);

	// create z object and line
	pLine = Create_Circle(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), l_cfRotateRingRadius, i_RenderLayer);
	Add_Line(pLine);
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pLine->SetOrientation(rot);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectRotate::~cmpsCompassObjectRotate()
{
	//std::vector<cmpsObjectSimple*>::iterator it;
	//for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	//	(*it)->RemoveFromScene(this->GetRenderLayer());

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

	//DBG_LOG1( "Scale = %5.2f", scale.GetX() );

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
bool cmpsCompassObjectRotate::RayPick(	int i_IconLayerIndex,
										const maPoint3d& i_RayStart,
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
	//maVector3d scale = this->GetScale();
	// get scale for just the icon layer we are picking in
	maVector3d scale = m_Lines[e_XIndex]->GetLayerScale(i_IconLayerIndex);

	// check sphere
	// doing this first causes it to always select sphere first
	// even if one of the axis objects is on top

	//DBG_LOG("----------------------------------------------------------------------");
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
		//DBG_LOG( "intersect: center" );

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

	//DBG_LOG( "--------------------------------------" );
	//DBG_LOG1( "ring width=%6.3f", l_cfRotateRingThickness );
	//DBG_LOG3( "arm_x(%6.3f,%6.3f,%6.3f)", arm_x.GetX(), arm_x.GetY(), arm_x.GetZ() );
	//DBG_LOG3( "arm_y(%6.3f,%6.3f,%6.3f)", arm_y.GetX(), arm_y.GetY(), arm_y.GetZ() );
	//DBG_LOG3( "arm_z(%6.3f,%6.3f,%6.3f)", arm_z.GetX(), arm_z.GetY(), arm_z.GetZ() );
	//DBG_LOG3( "scale(%6.3f,%6.3f,%6.3f)", GetScale().GetX(), GetScale().GetY(), GetScale().GetZ() );
	//DBG_LOG3( "cntr(%6.3f,%6.3f,%6.3f)", GetPosition().GetX(), GetPosition().GetY(), GetPosition().GetZ() );

	// check X
	if ( (GetParts() & cmpsCompass::e_X) &&
		 (geoRayIntersection::IntersectLineTorus(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_x,
			l_cfRotateRingRadius * scale.GetX(),
			l_cfRotateRingThickness * scale.GetX(),
			l_cfRotateRingThickness * scale.GetX(),
			t) )
		)
	{
		//DBG_LOG( "intersect: x" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_X;
			o_T = t;
		}
	}

	// check Y
	if ( (GetParts() & cmpsCompass::e_Y) &&
		 (geoRayIntersection::IntersectLineTorus(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_y,
			l_cfRotateRingRadius * scale.GetY(),
			l_cfRotateRingThickness * scale.GetY(),
			l_cfRotateRingThickness * scale.GetY(),
			t) )
		)
	{
		//DBG_LOG( "intersect: y" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_Y;
			o_T = t;
		}
	}

	// check Z
	if ( (GetParts() & cmpsCompass::e_Z) &&
		 (geoRayIntersection::IntersectLineTorus(
			i_RayStart,
			RaySeg,
			GetPosition(),
			arm_z,
			l_cfRotateRingRadius * scale.GetZ(),
			l_cfRotateRingThickness * scale.GetZ(),
			l_cfRotateRingThickness * scale.GetZ(),
			t))
		)
	{
		//DBG_LOG( "intersect: z" );

		intersection = true;
		if (o_T > t )
		{
			o_Part = e_Z;
			o_T = t;
		}
	}

	//DBG_LOG( "intersection = " << (intersection ? "true" : "false" ) );
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
//--------------------------------------------------------------------
//	SetLayerScale changes the scale of the compass for only a 
// given icon layer (a render panel)
//--------------------------------------------------------------------
void cmpsCompassObjectRotate::SetLayerScale(int i_IconLayerIndex, const maPoint3d& i_Scale)
{
	std::vector<cmpsObjectSimple*>::iterator it;
	for ( it = m_Lines.begin(); it != m_Lines.end(); ++it)
	{
		(*it)->SetLayerScale(i_IconLayerIndex, i_Scale);
	}
}

//--------------------------------------------------------------------
//	SetLayerRenderable sets the compass visibility for only a given icon 
// layer (a render panel)
//--------------------------------------------------------------------
void cmpsCompassObjectRotate::SetLayerRenderable(int i_IconLayerIndex, bool i_bRender)
{
	bool render_x = bool(i_bRender && (GetParts() & cmpsCompass::e_X) ? true : false);
	bool render_y = bool(i_bRender && (GetParts() & cmpsCompass::e_Y) ? true : false);
	bool render_z = bool(i_bRender && (GetParts() & cmpsCompass::e_Z) ? true : false);

	//DBG_LOG3( "renderable %d %d %d", render_x, render_y, render_z );

	m_Lines[e_XIndex]->SetLayerRenderable( i_IconLayerIndex, render_x );
	m_Lines[e_YIndex]->SetLayerRenderable( i_IconLayerIndex, render_y );
	m_Lines[e_ZIndex]->SetLayerRenderable( i_IconLayerIndex, render_z );
}

//----------------------------------------------------------------------------
//	Add_Line adds line to list
//----------------------------------------------------------------------------
void cmpsCompassObjectRotate::Add_Line( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT(i_pObject,"Object is null");
	m_Lines.push_back(i_pObject);
	//i_pObject->AddToScene( this->GetRenderLayer() );
}

//----------------------------------------------------------------------------
//	Create_Center creates a circle
//----------------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectRotate::Create_Circle(const maFloatRGBA& i_Color, float i_fRadius, 
									   cmpsRenderLayer::RenderLayer i_RenderLayer  )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCircle( i_fRadius, 72 );
	cmpsObjectSimple*	pCircle = new cmpsObjectSimple( pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer );
	pCircle->SetColor( i_Color );
	pCircle->ModifyEmissive( lc_CompassEmissiveNormal );
	pCircle->SetRenderable( true );
	return pCircle;
}

//--------------------------------------------------------------------
//	Resize the compass based on the given camera view
//--------------------------------------------------------------------
void cmpsCompassObjectRotate::ResizeObject(int i_IconLayerIndex, const camCamera& i_Camera )
{
	if (GetLockScale())
		return; 

	// A rotation will be around the object's pivot position, 
	// so it is necessary for the rotation compass to be centered on that origin.
	//
	maPoint3d origin = this->GetPivot();

	// Compute Camera Scale
	// Simple modifier by distance (only works for perspective cameras)
	//float dist = (i_Camera.GetPosition() - origin).Length();
	//const float c_Modifier = 0.15f;
	//const float default_size = GetDefaultSize();
	//float camera_scale = dist * c_Modifier / default_size; 

	// Use icnIconScale's function in order to handle orthographic cameras also
	const float c_Modifier = 1.5f;
	float camera_scale = c_Modifier * icnIconScale::GetIconScaleForPosition(origin, i_Camera);
	camera_scale *= this->GetScalingFactor(); // Comes from Increment/Decrement Compass commands

	// uniform scale
	float scale = camera_scale;
//	float scale = box.GetRadius() / default_size;
//	if (scale < camera_scale) scale = camera_scale;
	if (scale < l_cfMinScale) scale = l_cfMinScale;
	this->SetLayerScale( i_IconLayerIndex, maVector3d(scale, scale, scale) );
}


//----------------------------------------------------------------------------
//	AdjustCompassToBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectRotate::AdjustCompassToBounds( int i_IconLayerIndex,
													 const maPoint3d& i_Position, 
													 const maAxisBox& i_Bounds,
													 const maPoint3d& i_WorldPivot, 
												     const camCamera& i_Camera,
												     float i_ScalingFactor  )
{
	// First, let the parent set the actual bounds
	cmpsCompassObject::AdjustCompassToBounds( i_IconLayerIndex, i_Position, 
		i_Bounds, i_WorldPivot, i_Camera, i_ScalingFactor );

	// A rotation will be around the object's pivot position, 
	// so it is necessary for the rotation compass to be centered on that origin.
	//
	this->SetPosition(  this->GetPivot() );
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObjectRotate::GetDefaultSize() const
{
	return l_cfRotateRingRadius;
}


