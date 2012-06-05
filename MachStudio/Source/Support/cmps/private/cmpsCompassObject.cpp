/*****************************************************************************
**  cmpsCompassObject.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObject.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"


//--------------------------------------------------------------------
// Pass in render layer in scene to use for icons
//--------------------------------------------------------------------
cmpsCompassObject::cmpsCompassObject(cmpsRenderLayer::RenderLayer i_RenderLayer)
:	pick3dPickObject(),
	m_Position( 0.0f, 0.0f, 0.0f ),
	m_Scale( 1.0f,1.0f,1.0f ),
	m_Bounds( -2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f ),
	m_nShowOnlyParts( e_X | e_Y | e_Z | e_Center ),
	m_bRenderable(true),
	m_bLockScale(false),
	m_RenderLayer(i_RenderLayer)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObject::~cmpsCompassObject()
{
}


//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObject::SetPosition(const maPoint3d& i_Position)
{
	m_Position = i_Position;
}

//--------------------------------------------------------------------
//	LockScale() - don't allow the scale of the compass to change
//	anymore.
//--------------------------------------------------------------------
void cmpsCompassObject::LockScale( bool i_bLockScale )
{
	m_bLockScale = i_bLockScale;
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObject::SetScale(const maPoint3d& i_Scale)
{
	if ( m_bLockScale )
		return;

	this->m_Scale = i_Scale;

	//DBG_LOG1( "Scale = %5.2f", m_Scale.GetX() );

	// reset the object positions
	//SetPosition(m_Position);
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject::SetOrientation( const maRotation& i_Orientation )
{
	m_Orientation = i_Orientation;
}

//----------------------------------------------------------------------------
//	AdjustCompassToBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject::AdjustCompassToBounds( int i_IconLayerIndex,
											   const maPoint3d& i_Position, 
											   const maAxisBox& i_Bounds,
											   const maPoint3d& i_WorldPivot, 
											   const camCamera& i_Camera,
											   float i_ScalingFactor   )
{
	m_Position = i_Position;
	m_Bounds = i_Bounds;
	m_Pivot = i_WorldPivot;
	m_ScalingFactor = i_ScalingFactor;
}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompassObject::SetRenderable(bool i_bRender)
{
	m_bRenderable = i_bRender;
}

//--------------------------------------------------------------------
//	SetActive turns enabled state of the compass on and off.
//--------------------------------------------------------------------
void cmpsCompassObject::SetActive(bool i_bActive)
{
	m_bActive = i_bActive;
}

//----------------------------------------------------------------------------
//	SetParts turns on only the specified parts of the compass
//----------------------------------------------------------------------------
void cmpsCompassObject::SetParts( const int i_Parts )
{
	m_nShowOnlyParts = i_Parts;

	//DBG_LOG1( " SetParts %x", m_nShowOnlyParts );

	SetRenderable( m_bRenderable );
}


//--------------------------------------------------------------------
//	GetParts gets the currently rendered parts of the compass
//--------------------------------------------------------------------
int cmpsCompassObject::GetParts()
{
	return m_nShowOnlyParts;
}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObject::HighlightPart( int i_Part )
{
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompassObject.
//	If it does, the t value is also returned in o_T and the part that
//	was selected is returned in o_Part
//----------------------------------------------------------------------------
bool cmpsCompassObject::RayPick(int i_IconLayerIndex,
								const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								float& o_T,
								int& o_Part )
{
	if (!m_bRenderable)
		return false;

	return true;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmpsCompassObject::MatchPickCode(envType::UInt32 i_PickCode)
{
	return false;
}

//--------------------------------------------------------------------
// Return the index of the render layer in the scene to use for
//	the 3d icons of the compass
//--------------------------------------------------------------------
cmpsRenderLayer::RenderLayer cmpsCompassObject::GetRenderLayer()
{
	return m_RenderLayer;
}
