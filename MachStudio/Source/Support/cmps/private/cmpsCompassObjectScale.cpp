/*****************************************************************************
**  cmpsCompassObjectScale.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectScale.hpp"

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
	float l_fCubeHeight = 1;
	float l_fSphereRadius = 0.1f;
	float l_cfLineLength = 5.0f;
	float l_cfMinScale	= 0.5f;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectScale::cmpsCompassObjectScale(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompassObject3Axis(i_RenderLayer)
{
	//	for scale, the commented out code removes the visual lines
	//	this is to differentiate them from the other compasses.

	maRotation rot;
	maVector3d pos;
	cmpsObjectSimple* pObject;

	// create x object and line
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(1, 0, 0));
	pos.Set(l_cfLineLength,0.0f,0.0f);
	pObject = Create_Cylinder(maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	cmpsObjectSimple* pLine = Create_Line(cmpsCompassObject3Axis::e_AxisX, 
											maFloatRGBA( 1.0f, 0.0f, 0.0f, 1.0f ), 
											i_RenderLayer);
	Add_Line(pLine);

	// create y object and line
	rot.Identity();
	pos.Set(0.0f,l_cfLineLength,0.0f);
	pObject = Create_Cylinder(maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisY, 
						maFloatRGBA( 0.0f, 1.0f, 0.0f, 1.0f ), 
						i_RenderLayer);
	Add_Line(pLine);

	// create z object and line
	rot.SetValue( maVector3d(0, 1, 0), maVector3d(0, 0, 1));
	pos.Set(0.0f,0.0f,l_cfLineLength);
	pObject = Create_Cylinder(maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), rot, pos, i_RenderLayer);
	Add_Object(pObject);
	pLine = Create_Line(cmpsCompassObject3Axis::e_AxisZ, 
						maFloatRGBA( 0.0f, 0.0f, 1.0f, 1.0f ), 
						i_RenderLayer);
	Add_Line(pLine);

	// create the center sphere
	Add_Center(Create_Center(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_RenderLayer));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectScale::~cmpsCompassObjectScale()
{
}

//----------------------------------------------------------------------------
//	GetDefaultSize() - get the default size of the shape
//----------------------------------------------------------------------------
//virtual
float cmpsCompassObjectScale::GetDefaultSize() const
{
	return l_cfLineLength;
}

//----------------------------------------------------------------------------
//	AdjustCompassToBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectScale::AdjustCompassToBounds( int i_IconLayerIndex,
													const maPoint3d& i_Position, 
													const maAxisBox& i_Bounds,
												    const maPoint3d& i_WorldPivot, 
												    const camCamera& i_Camera,
												    float i_ScalingFactor  )
{
	// First, let the parent set the actual bounds
	cmpsCompassObject::AdjustCompassToBounds( i_IconLayerIndex, i_Position, 
		i_Bounds, i_WorldPivot, i_Camera, i_ScalingFactor );

	// A scale will be based on the object's pivot position, 
	// so it is necessary for the rotation compass to be centered on that origin.
	//
	maPoint3d pos( i_WorldPivot );
	this->SetPosition( pos );
}

//--------------------------------------------------------------------
//	Create_Cylinder creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectScale::Create_Cylinder( const maFloatRGBA& i_Color,
													       const maRotation& i_AxisRot,
													       const maVector3d& i_AxisPos,
														   cmpsRenderLayer::RenderLayer i_RenderLayer )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateCylinder( 0.5f, 0.5f, 1.0f, 10 );

	// Move the cone out to the end of an axis
	maMatrix4x4 matx = i_AxisRot.GetMatrix();
	matx.TranslateBy(i_AxisPos);

	cmpsObjectSimple* pObject = new cmpsObjectSimple( matx, pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer );
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );
	return pObject;
}


