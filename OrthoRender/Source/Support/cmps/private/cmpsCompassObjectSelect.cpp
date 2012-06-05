/*****************************************************************************
**  cmpsCompassObjectSelect.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectSelect.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
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
	const float l_cfSelectBoxDefaultSize = 4.0f;

	float l_fConeHeight = 5.0f;
	float l_fConeRadius = 1.0f;
	float l_fSphereRadius = 0.1f;
	float l_fLineLength = 5.0f;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectSelect::cmpsCompassObjectSelect(int i_RenderLayer)
:	cmpsCompassObjectSingle(i_RenderLayer)
{
	maRotation rot;

	// create the Object
	Set_Object(Create_Block(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), l_cfSelectBoxDefaultSize, l_cfSelectBoxDefaultSize, l_cfSelectBoxDefaultSize));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsCompassObjectSelect::~cmpsCompassObjectSelect()
{
}

//--------------------------------------------------------------------
//	Recreate the selection box for the object
//--------------------------------------------------------------------
void cmpsCompassObjectSelect::RecreateObject( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize )
{
	//	first get rid of the object
	Delete_Object();

	// create the Object
	Set_Object(Create_Block(maFloatRGBA( 1.0f, 1.0f, 0.0f, 1.0f ), i_XSize, i_YSize, i_ZSize));
}

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
void cmpsCompassObjectSelect::SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos  )
{
	// First, let the parent set the actual bounds
	cmpsCompassObjectSingle::SetBounds( i_Bounds, i_WorldPivot, i_CameraPos );

	// now we need to scale and position the object based on the bounds.
	//
	ResizeObject(i_CameraPos);
}

//--------------------------------------------------------------------
//	Create_Block creates a object
//--------------------------------------------------------------------
cmpsObjectSimple* cmpsCompassObjectSelect::Create_Block( const maFloatRGBA& i_Color, const float i_XSize, const float i_YSize, const float i_ZSize )
{
	g3dFragment* pFragment = g3dPrimitiveFragmentUtil::CreateLineBlock( i_XSize, i_YSize, i_ZSize );
	cmpsObjectSimple*	pObject = new cmpsObjectSimple( pFragment, 
		new matMaterial("Phong.fx") );
	pObject->SetColor( i_Color );
	pObject->ModifyEmissive( lc_CompassEmissiveNormal );
	pObject->SetRenderable( true );

	return pObject;
}


//--------------------------------------------------------------------
//	using the latest compass settings, modify the object's shape.
//--------------------------------------------------------------------
void cmpsCompassObjectSelect::ResizeObject(const maPoint3d& i_CameraPos)
{
	maAxisBox box	= GetBounds();

	//	set the select box to be the center of the bounding area
	//	(ignoring the object's position)
	//
	maPoint3d pos( box.GetCenter() );
	cmpsCompassObjectSingle::SetPosition( pos );

	// Compute Camera Scale
	float dist = (i_CameraPos - pos).Length();
	const float c_Modifier = 0.15f;
	float camera_scale = dist * c_Modifier / l_cfSelectBoxDefaultSize; // could be some formula of distance


	//DBG_LOG3( "max (%6.3f,%6.3f,%6.3f)",  box.GetMaxX(), box.GetMaxY(), box.GetMaxZ() );
	//DBG_LOG3( "min (%6.3f,%6.3f,%6.3f)",  box.GetMinX(), box.GetMinY(), box.GetMinZ() );
	//DBG_LOG3( "diff(%6.3f,%6.3f,%6.3f)",  box.GetDiffX(), box.GetDiffY(), box.GetDiffZ() );
	//DBG_LOG3( "cntr(%6.3f,%6.3f,%6.3f)", box.GetCenter().GetX(), box.GetCenter().GetY(), box.GetCenter().GetZ() );

	//	scale the object accordingly
	float scalex = box.GetDiffX() / l_cfSelectBoxDefaultSize;
	if (scalex < camera_scale) scalex = camera_scale;
	float scaley = box.GetDiffY() / l_cfSelectBoxDefaultSize;
	if (scaley < camera_scale) scaley = camera_scale;
	float scalez = box.GetDiffZ() / l_cfSelectBoxDefaultSize;
	if (scalez < camera_scale) scalez = camera_scale;

	//temporary
	if (scalex == 0.0f) scalex = 1.0f;
	if (scaley == 0.0f) scaley = 1.0f;
	if (scalez == 0.0f) scalez = 1.0f;
	//DBG_LOG3( "select compass scale (%6.3f,%6.3f,%6.3f)", scalex, scaley, scalez );

	cmpsCompassObjectSingle::SetScale( maVector3d( scalex, scaley, scalez ) );
}

