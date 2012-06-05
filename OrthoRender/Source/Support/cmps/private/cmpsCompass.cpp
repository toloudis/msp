/*****************************************************************************
**  cmpsCompass.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/cmpsCompass.hpp"
#include "Support/cmps/private/cmpsCompassObject.hpp"

#include "MainApp/mnmApp.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"


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
	float l_fSphereRadius	= 0.2f;
	float l_fLineLength		= 5.0f;
	float l_fRayPickWidth	= 0.5f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompass::cmpsCompass(RenderLayer i_RenderLayer)
:	pick3dPickObject(),
	m_pCompassObject(NULL),
	m_RenderLayer(i_RenderLayer)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompass::~cmpsCompass()
{
	// Note: the child classes should delete their compass objects.
}

//----------------------------------------------------------------------------
//	GetPosition returns the position of the compass.
//----------------------------------------------------------------------------
const maPoint3d& cmpsCompass::GetPosition() const
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->GetPosition();
}


//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsCompass::SetPosition(const maPoint3d& i_Position)
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->SetPosition( i_Position );
}

//--------------------------------------------------------------------
//	LockScale() - don't allow the scale of the compass to change
//	anymore.
//--------------------------------------------------------------------
void cmpsCompass::LockScale( bool i_bLockScale )
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->LockScale( i_bLockScale );
}

//--------------------------------------------------------------------
//	GetScale the scale of the compass.
//--------------------------------------------------------------------
const maPoint3d& cmpsCompass::GetScale()
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->GetScale();
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompass::SetScale(const maPoint3d& i_Scale)
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->SetScale( i_Scale );
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompass::SetOrientation( const maRotation& i_Orientation )
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->SetOrientation( i_Orientation );
}

//----------------------------------------------------------------------------
//	SetBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompass::SetBounds( const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos  )
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );
	//DBG_LOG6( "bounds (%6.3f,%6.3f)  (%6.3f,%6.3f)  (%6.3f,%6.3f)", i_Bounds.GetMinX(), i_Bounds.GetMaxX(), i_Bounds.GetMinY(), i_Bounds.GetMaxY(), i_Bounds.GetMinZ(), i_Bounds.GetMaxZ() );

	return m_pCompassObject->SetBounds( i_Bounds, i_WorldPivot, i_CameraPos );
}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompass::SetRenderable(bool i_bRender)
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->SetRenderable( i_bRender );
}

//----------------------------------------------------------------------------
//	GetRenderable
//----------------------------------------------------------------------------
bool cmpsCompass::GetRenderable()
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->GetRenderable();
}

//----------------------------------------------------------------------------
//	SetActive turns enabled state of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompass::SetActive(bool i_bActive)
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );
	return m_pCompassObject->SetActive( i_bActive );
}

//----------------------------------------------------------------------------
//	GetActive
//----------------------------------------------------------------------------
bool cmpsCompass::GetActive()
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );
	return m_pCompassObject->GetActive();
}

//----------------------------------------------------------------------------
//	SetParts turns on only the specified parts of the compass
//----------------------------------------------------------------------------
void cmpsCompass::SetParts(const int i_Parts)
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->SetParts(i_Parts);
}

//--------------------------------------------------------------------
//	GetParts gets the currently rendered parts of the compass
//--------------------------------------------------------------------
int cmpsCompass::GetParts()
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->GetParts();
}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompass::HighlightPart( int i_Part )
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->HighlightPart(i_Part);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompass.
//	If it does, the t value is also returned in o_T and the part that
//	was selected is returned in o_Part
//----------------------------------------------------------------------------
bool cmpsCompass::RayPick(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							float& o_T,
							int& o_Part )
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	return m_pCompassObject->RayPick( i_RayStart, i_RayEnd, o_T, o_Part );
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* cmpsCompass::MatchPickCode(envType::UInt32 i_PickCode) const
{
	DBG_ASSERT0( m_pCompassObject, "No compass object" );

	if (m_pCompassObject->MatchPickCode( i_PickCode ) )
		return m_pCompassObject;

	return NULL;
}

//--------------------------------------------------------------------
// Return the index of the render layer in the scene to use for
//	the 3d icons of the compass
//--------------------------------------------------------------------
int cmpsCompass::GetObjectRenderLayer()
{
	switch (m_RenderLayer)
	{
	default:
	case e_World:
		return mnmApp::GetIconsLayerIndex();
		//return api3dScene::WorldLayerIndex();
	case e_Camera:
		return api3dScene::CameraLayerIndex();
	}
}

