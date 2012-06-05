/*****************************************************************************
**  cmpsCompassObjectSelect.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectSelect.hpp"

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

//	tool library
#include "Tool/cam3d/cam3dMgr.hpp"


#include <algorithm>


namespace
{
	const float l_cfSelectBoxDefaultSize = 1.0f;

	float l_fConeHeight = 5.0f;
	float l_fConeRadius = 1.0f;
	float l_fSphereRadius = 0.1f;
	float l_fLineLength = 5.0f;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectSelect::cmpsCompassObjectSelect(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	cmpsCompassObjectSingle(i_RenderLayer)
{
	maRotation rot;

	// create the Object
	Set_Object(Create_Block(maFloatRGBA( 0.95f, 0.95f, 0.65f, 1.0f ), 
							l_cfSelectBoxDefaultSize, 
							l_cfSelectBoxDefaultSize, 
							l_cfSelectBoxDefaultSize,
							i_RenderLayer));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectSelect::~cmpsCompassObjectSelect()
{
}

//--------------------------------------------------------------------
//	Recreate the selection box for the object
//--------------------------------------------------------------------
//void cmpsCompassObjectSelect::RecreateObject( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize )
//{
//	//	first get rid of the object
//	Delete_Object();
//
//	// create the Object
//	Set_Object(Create_Block(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_XSize, i_YSize, i_ZSize));
//}

//--------------------------------------------------------------------
//	SetPosition  moves the compass to the position passed in
//--------------------------------------------------------------------
void cmpsCompassObjectSelect::SetPosition(const maPoint3d& i_Position)
{
	// First, let the parent set the actual bounds
	cmpsCompassObjectSingle::SetPosition( i_Position );

	// now we need to scale and position the object based on the bounds.
	//
	//ResizeObject();
}

//----------------------------------------------------------------------------
//	SetBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
void cmpsCompassObjectSelect::AdjustCompassToBounds( int i_IconLayerIndex,
													 const maPoint3d& i_Position, 
													 const maAxisBox& i_Bounds,
												     const maPoint3d& i_WorldPivot, 
												     const camCamera& i_Camera,
												     float i_ScalingFactor   )
{
	// First, let the parent set the actual bounds
	cmpsCompassObjectSingle::AdjustCompassToBounds( i_IconLayerIndex, i_Position, 
		i_Bounds, i_WorldPivot, i_Camera, i_ScalingFactor );

	//	set the select box to be the center of the bounding area
	//	(ignoring the object's position)
	//
	maPoint3d pos( i_Bounds.GetCenter() );
	this->SetPosition( pos );

	// Compute Camera Scale
	float dist = (i_Camera.GetPosition() - pos).Length();
	float camera_scale = 1.0f;		//fixed scale of bounds (global scale overridden)
	camera_scale *= this->GetScalingFactor(); // Comes from Increment/Decrement Compass commands

	// Set scale for this layer only
	this->SetLayerScale( i_IconLayerIndex, 
						maVector3d( i_Bounds.GetDiffX()*camera_scale, 
									i_Bounds.GetDiffY()*camera_scale, 
									i_Bounds.GetDiffZ()*camera_scale ) );
}

//--------------------------------------------------------------------
//	Create_Block creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectSelect::Create_Block( const maFloatRGBA& i_Color, 
				const float i_XSize, const float i_YSize, const float i_ZSize,
				cmpsRenderLayer::RenderLayer i_RenderLayer)
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateLineBlock( i_XSize, i_YSize, i_ZSize );
	cmpsObjectSimple*	pObject = new cmpsObjectSimple( pFragment, 
		new matMaterial("Solid.fx"), i_RenderLayer );
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );

	return pObject;
}


//--------------------------------------------------------------------
//	Resize the compass based on the given camera view
//--------------------------------------------------------------------
void cmpsCompassObjectSelect::ResizeObject(int i_IconLayerIndex,
										   const camCamera& i_Camera )
{
	// No resizing per camera is necessary for the selection box,
	// it stays the same size based on the bounds of the selection
}

