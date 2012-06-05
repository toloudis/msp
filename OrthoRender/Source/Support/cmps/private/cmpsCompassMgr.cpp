/*****************************************************************************
**  cmpsCompassMgr.cpp
**
**      The cmpsCompassMgr manages all the fog for the level
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#include "Support/cmps/cmpsCompassMgr.hpp"

#include "Support/cmps/private/cmpsCompassScale.hpp"
#include "Support/cmps/private/cmpsCompassSelect.hpp"
#include "Support/cmps/private/cmpsCompassRotate.hpp"
#include "Support/cmps/private/cmpsCompassTranslate.hpp"

//	library
#include "Tool/api3d/api3dScene.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maConstants.hpp"

//	tool
#include "Tool/pick3d/pick3dMgr.hpp"

//	standard
#include <map>


namespace
{
	typedef std::map<int, cmpsCompass*> CompassMap;
	CompassMap l_Compasses;

	float l_fCompassRadian			= 3 * maConstants::c_fAngleToRad;
	float l_fScreenCompassRadian	= 0.02f;
}


//----------------------------------------------------------------------------
//	Initialize will be called when the level is new and default fog
//	should be added
//----------------------------------------------------------------------------
void cmpsCompassMgr::Initialize()
{
	//	set-up the compasses
	l_Compasses[ e_Translate ] = new cmpsCompassTranslate();
	l_Compasses[ e_Translate ]->SetRenderable(false);

	l_Compasses[ e_Scale ] = new cmpsCompassScale();
	l_Compasses[ e_Scale ]->SetRenderable(false);

	l_Compasses[ e_Rotate ] = new cmpsCompassRotate();
	l_Compasses[ e_Rotate ]->SetRenderable(false);

	const bool axis_aligned = false;	// allow icon to rotate with camera
	l_Compasses[ e_World ] = new cmpsCompassTranslate(cmpsCompass::e_Camera, axis_aligned);
	l_Compasses[ e_World ]->SetRenderable(true);
	l_Compasses[ e_World ]->SetScale( maPoint3d( l_fScreenCompassRadian, l_fScreenCompassRadian, l_fScreenCompassRadian ) );
	l_Compasses[ e_World ]->LockScale( true );
	l_Compasses[ e_World ]->SetPosition( maPoint3d(2.1f,-1.6f,2.5f) );

	l_Compasses[ e_Select ] = new cmpsCompassSelect();
	l_Compasses[ e_Select ]->SetRenderable(false);
}

//----------------------------------------------------------------------------
//	DeInitialize - remove Pick Interest and remove compasses from the level
//----------------------------------------------------------------------------
void cmpsCompassMgr::DeInitialize()
{
	//	delete all the compasses
	CompassMap::iterator it = l_Compasses.begin();
	CompassMap::iterator end = l_Compasses.end();

	for ( ; it != end; ++it )
	{
		delete (*it).second;
	}

	l_Compasses.clear();
}

//----------------------------------------------------------------------------
//	Think updates compass based on the camera position
//----------------------------------------------------------------------------
void cmpsCompassMgr::Think( const maPoint3d& i_CamPos )
{
	//float length = 0.0f;
	//float scale	 = 0.0f;
	//
	//CompassMap::iterator it  = l_Compasses.begin();
	//CompassMap::iterator end = l_Compasses.end();
	//
	//for ( ; it != end; ++it )
	//{
	//	if ( (*it).second->GetRenderable() )
	//	{
	//		maVector3d dist = i_CamPos - (*it).second->GetPosition();
	//		length = dist.Length();
	//		scale = atan(l_fCompassRadian) * length;
	//		(*it).second->SetScale( maPoint3d(scale,scale,scale) );
	//	}
	//}
}

//----------------------------------------------------------------------------
//	return the number of compasses held by the compass manager
//----------------------------------------------------------------------------
int cmpsCompassMgr::GetNumberOfCompasses()
{
	return l_Compasses.size();
}

//----------------------------------------------------------------------------
//	GetCompass returns the compass
//----------------------------------------------------------------------------
cmpsCompass& cmpsCompassMgr::GetCompass( int i_CompassType )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	return *(l_Compasses[i_CompassType]);
}

//----------------------------------------------------------------------------
//	SetPosition sets the compass position
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetPosition( int i_CompassType, const maPoint3d& i_Position )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetPosition(i_Position);
}

//----------------------------------------------------------------------------
//	SetOrientation sets the compass orientation
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetOrientation( int i_CompassType, const maRotation& i_Orientation )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetOrientation(i_Orientation);
}

//----------------------------------------------------------------------------
//	SetBounds sets the bounding area for this compass
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetBounds( int i_CompassType, 
							   const maAxisBox& i_Bounds,
							   const maPoint3d& i_WorldPivot, 
							   const maPoint3d& i_CameraPos  )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetBounds( i_Bounds, i_WorldPivot, i_CameraPos );
}

//----------------------------------------------------------------------------
//	SetRenderable sets all compass visibility
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetRenderable( bool i_Renderable )
{
	CompassMap::iterator it = l_Compasses.begin();
	CompassMap::iterator end = l_Compasses.end();

	for ( ; it != end; ++it )
	{
		(*it).second->SetRenderable( i_Renderable );
	}
}

//----------------------------------------------------------------------------
//	SetRenderable sets the compass visibility
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetRenderable( int i_CompassType, bool i_Renderable )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetRenderable(i_Renderable);
}
bool cmpsCompassMgr::GetRenderable( int i_CompassType )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	return l_Compasses[i_CompassType]->GetRenderable();
}

//----------------------------------------------------------------------------
//	SetActive sets the compass enabled state
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetActive( int i_CompassType, bool i_Active )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetActive(i_Active);
}

//----------------------------------------------------------------------------
//	SetParts
//----------------------------------------------------------------------------
void cmpsCompassMgr::SetParts( int i_CompassType, const int i_Parts )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->SetParts(i_Parts);
}

//----------------------------------------------------------------------------
//	GetParts
//----------------------------------------------------------------------------
int cmpsCompassMgr::GetParts( int i_CompassType )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	return l_Compasses[i_CompassType]->GetParts();
}

//----------------------------------------------------------------------------
//	HighlightPart - highlight a part of a compass
//----------------------------------------------------------------------------
void cmpsCompassMgr::HighlightPart( int i_CompassType, int i_Part )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	l_Compasses[i_CompassType]->HighlightPart( i_Part );
	//DBG_LOG1( "Highlight part #%d", i_Part );
}

//----------------------------------------------------------------------------
//	PickedCompassPart returns true if the given ray intersects a cmpsCompass.
//	return: the intersect point is returned in o_PickPos
//			the axis that was selected is returned in o_Axis
//----------------------------------------------------------------------------
bool cmpsCompassMgr::PickedCompassPart(	const maPoint3d& i_RayStart,
										const maPoint3d& i_RayEnd,
										maPoint3d& o_PickPos,
										int& o_CompassType,
										int& o_Part )
{
	int i = 0;
	for ( ; i < l_Compasses.size() ; ++i )
	{
		if ( RayPick( i, i_RayStart, i_RayEnd, o_PickPos, o_Part ) )
		{
			o_CompassType = i;
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
//	RayPick returns true if the given ray intersects the cmpsCompass.
//	If it does, the t value is also returned in o_T and the axis that
//	was selected is returned in o_Axis
//----------------------------------------------------------------------------
bool cmpsCompassMgr::RayPick(	int i_CompassType,
								const maPoint3d& i_RayStart,
								const maPoint3d& i_RayEnd,
								maPoint3d& o_PickPos,
								int& o_Part )
{
	DBG_ASSERT0( i_CompassType >= 0 && i_CompassType < l_Compasses.size(),"i_CompassType out of range" );

	float t = 2.0f;
	bool intersect = l_Compasses[i_CompassType]->RayPick(i_RayStart, i_RayEnd, t, o_Part );

	if (intersect)
	{
		o_PickPos = i_RayStart + (i_RayEnd - i_RayStart) * t;
		//DBG_LOG1( "Picked part #%d", o_Part );
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
pick3dPickObject* cmpsCompassMgr::MatchPickCode(envType::UInt32 i_PickCode) 
{	
	for (int i=0 ; i < l_Compasses.size() ; ++i )
	{
		pick3dPickObject* pPicked = l_Compasses[i]->MatchPickCode(i_PickCode);
		if (pPicked)
			return pPicked;
	}
	return NULL;
}
