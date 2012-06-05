/*****************************************************************************
**  cmpsCompassMgr.hpp
**
**      The cmpsCompassMgr keeps track of compasses for modes
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_COMPASSMGR_HPP
#error cmpsCompassMgr.hpp multiply included
#endif
#define CMPS_COMPASSMGR_HPP

#ifndef CMPS_COMPASS_HPP
#include "Support/cmps/cmpsCompass.hpp"
#endif


//============================================================================
//============================================================================
namespace cmpsCompassMgr
{
	enum Compasses
	{
		e_Rotate = 0,
		e_Scale,
		e_Translate,
		e_World,
		e_Select,
		e_DirLightRotate
	};

	//----------------------------------------------------------------------------
	//	Initialize
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//	removes
	//----------------------------------------------------------------------------
	void DeInitialize();

	//----------------------------------------------------------------------------
	//	Think updates compass based on the camera position
	//----------------------------------------------------------------------------
	void Think( const maPoint3d& i_CamPos );

	//----------------------------------------------------------------------------
	//	return the number of compasses held by the compass manager
	//----------------------------------------------------------------------------
	int GetNumberOfCompasses();

	//----------------------------------------------------------------------------
	//	SetPosition sets the compass position
	//----------------------------------------------------------------------------
	void SetPosition( int i_CompassType, const maPoint3d& i_Position );

	//----------------------------------------------------------------------------
	//	SetOrientation sets the compass orientation
	//----------------------------------------------------------------------------
	void SetOrientation( int i_CompassType, const maRotation& i_Orientation );

	//----------------------------------------------------------------------------
	//	SetBounds sets the bounding area for this compass
	//----------------------------------------------------------------------------
	void SetBounds( int i_CompassType, 
					const maAxisBox& i_Bounds, 
					const maPoint3d& i_WorldPivot, 
					const maPoint3d& i_CameraPos  );

	//----------------------------------------------------------------------------
	//	SetRenderable sets all compass visibility
	//----------------------------------------------------------------------------
	void SetRenderable( bool i_Renderable );

	//----------------------------------------------------------------------------
	//	SetRenderable sets the compass visibility
	//----------------------------------------------------------------------------
	void SetRenderable( int i_CompassType, bool i_Renderable );
	bool GetRenderable( int i_CompassType );

	//----------------------------------------------------------------------------
	//	SetActive sets the compass enabled state
	//----------------------------------------------------------------------------
	void SetActive( int i_CompassType, bool i_Active );

	//----------------------------------------------------------------------------
	//	SetParts
	//----------------------------------------------------------------------------
	void SetParts( int i_CompassType, const int i_Parts );

	//----------------------------------------------------------------------------
	//	GetParts
	//----------------------------------------------------------------------------
	int GetParts( int i_CompassType );

	//----------------------------------------------------------------------------
	//	HighlightPart - highlight a part of a compass
	//----------------------------------------------------------------------------
	void HighlightPart( int i_CompassType, int i_Part );

	//----------------------------------------------------------------------------
	//	PickedCompassPart returns true if the given ray intersects a cmpsCompass.
	//	return: the intersect point is returned in o_PickPos
	//			the axis that was selected is returned in o_Axis
	//----------------------------------------------------------------------------
	bool PickedCompassPart(	const maPoint3d& i_RayStart,
							const maPoint3d& i_RayEnd,
							maPoint3d& o_PickPos,
							int& o_CompassType,
							int& o_Part );

	//----------------------------------------------------------------------------
	//	RayPick returns true if the given ray intersects the cmpsCompass.
	//	If it does, the intersect point is returned in o_PickPos and the axis that
	//	was selected is returned in o_Axis
	//----------------------------------------------------------------------------
	bool RayPick(	int i_CompassType,
					const maPoint3d& i_RayStart,
					const maPoint3d& i_RayEnd,
					maPoint3d& o_PickPos,
					int& o_Part);

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode);

	//----------------------------------------------------------------------------
	//	GetCompass returns the compass
	//----------------------------------------------------------------------------
	cmpsCompass& GetCompass( int i_CompassType );
};
