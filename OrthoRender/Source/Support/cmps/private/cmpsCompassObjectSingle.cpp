/*****************************************************************************
**  cmpsCompassObjectSingle.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/private/cmpsCompassObjectSingle.hpp"

#include "Support/cmps/private/cmpsObjectSimple.hpp"

#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dFragmentCreate.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Core/geo/geoRayIntersection.hpp"
#include "Core/ma/maConstants.hpp"
#include "Core/ma/maFunctions.hpp"

#include <algorithm>


//============================================================================
//============================================================================
namespace
{
	float l_fObjectHeight	= 1.0f;
	//float l_fObjectRadius	= 0.3f;
	float l_fSphereRadius	= 0.1f;
	float l_fLineLength		= 5.0f;
	float l_fRayPickWidth	= 0.5f;

	const maFloatRGBA lc_HighlightColor(1.0f,1.0f,1.0f,1.0f);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObjectSingle::cmpsCompassObjectSingle(int i_RenderLayer)
:	cmpsCompassObject(i_RenderLayer),
	m_pObject(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmpsCompassObjectSingle::~cmpsCompassObjectSingle()
{
	Delete_Object();
}


//----------------------------------------------------------------------------
//	SetPosition sets the position of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObjectSingle::SetPosition(const maPoint3d& i_Position)
{
	cmpsCompassObject::SetPosition( i_Position );

	if (m_pObject)
	{
		m_pObject->SetPosition( GetPosition() );
	}
}

//----------------------------------------------------------------------------
//	SetScale changes the scale of the compass.
//----------------------------------------------------------------------------
void cmpsCompassObjectSingle::SetScale(const maPoint3d& i_Scale)
{
	if ( GetLockScale() )
		return;

	cmpsCompassObject::SetScale( i_Scale );

	if (m_pObject)
	{
		m_pObject->SetScale(GetScale());
	}

	//DBG_LOG1( "Scale = %5.2f", GetScale().GetX() );

	// reset the object positions
	//SetPosition(GetPosition());
}

//----------------------------------------------------------------------------
//	SetOrientation changes the orientation of the compass.
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectSingle::SetOrientation( const maRotation& i_Orientation )
{
	cmpsCompassObject::SetOrientation( i_Orientation );

	m_pObject->SetOrientation(i_Orientation);

	// reset the object positions
	SetPosition(GetPosition());
}

//----------------------------------------------------------------------------
//	SetRenderable turns display of the compass on and off.
//----------------------------------------------------------------------------
void cmpsCompassObjectSingle::SetRenderable(bool i_bRender)
{
	cmpsCompassObject::SetRenderable( i_bRender );

	if ( m_pObject )
	{
		m_pObject->SetRenderable( i_bRender );
	}
}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
//virtual
void cmpsCompassObjectSingle::HighlightPart( int i_Part )
{
	//	set the appropriate emissive piece
	//	Set the color 
	m_pObject->ModifyColor( ((i_Part != -1) ? (lc_HighlightColor) : (m_pObject->GetColor())) );
}

//--------------------------------------------------------------------
//	SetActive turns enabled state of the compass on and off.
//--------------------------------------------------------------------
//virtual 
void cmpsCompassObjectSingle::SetActive(bool i_bActive)
{
	cmpsCompassObject::SetActive( i_bActive );
	m_pObject->DisplayActive(i_bActive);
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompassObject.
//	If it does, the t value is also returned in o_T and the part that
//	was selected is returned in o_Part
//----------------------------------------------------------------------------
bool cmpsCompassObjectSingle::RayPick(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							float& o_T )
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
	//DBG_LOG2( "center=%08x    ptr=%x", (GetParts() & e_Center), m_pObject );

	if (  m_pObject
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
		o_T = t;
	}

	//DBG_LOG1( "intersection = %s", (intersection ? "true" : "false" ) );
	return intersection;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
bool cmpsCompassObjectSingle::MatchPickCode(envType::UInt32 i_PickCode)
{
	if (!GetRenderable())
		return false;

	if ( m_pObject )
	{
		if (m_pObject->ContainsPickCode( i_PickCode ) )
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//	Set_Object sets internal object pointer
//----------------------------------------------------------------------------
void cmpsCompassObjectSingle::Set_Object( cmpsObjectSimple* i_pObject )
{
	DBG_ASSERT0(i_pObject,"Object is null");
	DBG_ASSERT0(!m_pObject,"object already exists");
	m_pObject = i_pObject;
	api3dScene::AddObject( m_pObject, this->GetRenderLayer() );
}

//--------------------------------------------------------------------
//	Delete_Object - deletes the current objects held by this class
//--------------------------------------------------------------------
//virtual
void cmpsCompassObjectSingle::Delete_Object()
{
	api3dScene::RemoveObject( m_pObject, this->GetRenderLayer() );
	delete m_pObject;
	m_pObject = NULL;
}


