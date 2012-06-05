/*****************************************************************************
**  cmpsCompassObjectTranslate.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectTranslate.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/eff/effPhongData.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Support/cmps/private/cmpsObjectSimple.hpp"
#include "Graphics/g3d/g3dScene.hpp"

#include <algorithm>

namespace
{
	float l_fConeHeight = 1;
	float l_fConeRadius = 0.3f;
	float l_fSphereRadius = 0.1f;
	float l_fLineLength = 5.0f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectTranslate::cmpsCompassObjectTranslate(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompassObject3Axis(i_RenderLayer)
{
	maRotation rot;
	maVector3d pos;

	// create x object and line
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pos.Set(l_fLineLength,0.0f,0.0f);
	cmpsObjectSimple* pObject = Create_Cone(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	cmpsObjectSimple* pLine = Create_Line(cmpsCompassObject3Axis::e_AxisX, maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), i_RenderLayer);
	Add_Line(pLine);

	// create y object and line
	rot.Identity();
	pos.Set(0.0f,l_fLineLength,0.0f);
	pObject = Create_Cone(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisY, maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer);
	Add_Line(pLine);

	// create y object and line
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pos.Set(0.0f,0.0f,l_fLineLength);
	pObject = Create_Cone(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisZ, maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), i_RenderLayer);
	Add_Line(pLine);

	// create the center sphere
	Add_Center(Create_Center(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer));

	// create rectangle for planar motion
	cmpsObjectSimple* pPlane = Create_Plane(cmpsCompassObject3Axis::e_RectXY, maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer);
	Add_Plane(pPlane);
	pPlane = Create_Plane(cmpsCompassObject3Axis::e_RectXZ, maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer);
	Add_Plane(pPlane);
	pPlane = Create_Plane(cmpsCompassObject3Axis::e_RectYZ, maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer);
	Add_Plane(pPlane);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectTranslate::~cmpsCompassObjectTranslate()
{
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObjectTranslate::GetDefaultSize() const
{
	return l_fLineLength;
}

//----------------------------------------------------------------------------
//	AdjustCompassToBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectTranslate::AdjustCompassToBounds( int i_IconLayerIndex,
													const maPoint3d& i_Position, 
													const maAxisBox& i_Bounds,
												    const maPoint3d& i_WorldPivot, 
												    const camCamera& i_Camera,
												    float i_ScalingFactor  )
{
	// First, let the parent set the actual bounds
	cmpsCompassObject::AdjustCompassToBounds( i_IconLayerIndex, i_Position, 
		i_Bounds, i_WorldPivot, i_Camera, i_ScalingFactor );

	//	set the compass to be the center of the bounding area
	//	(ignoring the object's position).
	// If bbox is empty, use world pivot location (useful for parent nodes and lights)
	//
	if (!i_Bounds.IsEmpty())
		this->SetPosition( i_Bounds.GetCenter() );
	else
		this->SetPosition( i_WorldPivot );	
}


//--------------------------------------------------------------------
//	Create_Cone creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectTranslate::Create_Cone( const maFloatRGBA& i_Color,
														   const maRotation& i_AxisRot,
														   const maVector3d& i_AxisPos,
														   cmpsRenderLayer::RenderLayer i_RenderLayer )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCone( l_fConeRadius, l_fConeHeight, 15 );

	// Move the cone out to the end of an axis
	maMatrix4x4 matx = i_AxisRot.GetMatrix();
	matx.TranslateBy(i_AxisPos);

	cmpsObjectSimple* pObject = new cmpsObjectSimple( matx, pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer);
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );
	return pObject;
}

