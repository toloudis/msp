/****************************************************************************\
**	pntPickInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Support/pnt/pntPickInterest.hpp"

#include "Support/pnt/pntPointSelect.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
pntPickInterest::pntPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
pntPickInterest::~pntPickInterest()
{
}

//--------------------------------------------------------------------
//	OccluderPick() - search through objects that are occluders and
//	modify the tVal so the ray is shortened to only the visible length
//--------------------------------------------------------------------
//virtual
bool pntPickInterest::OccluderPick(geoPickRay& i_Ray, float &o_tVal)
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
bool pntPickInterest::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	return pntPointSelect::ObjectPick(i_Ray, io_PickList);
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* pntPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return pntPointSelect::MatchPickCode(i_PickCode);
}
