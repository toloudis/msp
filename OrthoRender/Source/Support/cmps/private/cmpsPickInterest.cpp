/****************************************************************************\
**	cmpsPickInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsPickInterest.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"

#include "Core/geo/geoPickRay.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmpsPickInterest::cmpsPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
cmpsPickInterest::~cmpsPickInterest()
{
}

//--------------------------------------------------------------------
//	OccluderPick() - search through objects that are occluders and
//	modify the tVal so the ray is shortened to only the visible length
//--------------------------------------------------------------------
//virtual
bool cmpsPickInterest::OccluderPick(geoPickRay& i_Ray, float &o_tVal)
{
	return false;
}

//--------------------------------------------------------------------
//	ObjectPick() - after the OccluderPick() is called (optional) the
//	shortened ray can then be used for actual object picking.  The
//	objects in the list are sorted from the closest to the farthest.
//	So getting the first object in the list will be the nearest.
//--------------------------------------------------------------------
//virtual
bool cmpsPickInterest::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	maPoint3d pickedPt;
	int compassType;
	int compassPart;

	bool bRetVal = cmpsCompassMgr::PickedCompassPart( i_Ray.GetRayStart(),
													  i_Ray.GetRayEnd(),
													  pickedPt,
													  compassType,
													  compassPart );
	if ( bRetVal )
	{
		//	found one, so add it to the list
		io_PickList.AddItem( &cmpsCompassMgr::GetCompass( compassType ), 1.0f );
	}
	return bRetVal;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* cmpsPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return cmpsCompassMgr::MatchPickCode(i_PickCode);
}