/****************************************************************************\
**	setsPickInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/setsPickInterest.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
setsPickInterest::setsPickInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
setsPickInterest::~setsPickInterest()
{
}

//--------------------------------------------------------------------
//	OccluderPick() - search through objects that are occluders and
//	modify the tVal so the ray is shortened to only the visible length
//--------------------------------------------------------------------
//virtual
bool setsPickInterest::OccluderPick(geoPickRay& i_Ray, float &o_tVal)
{
	setsDataMgr::OccluderPick( i_Ray,  o_tVal );
	return false;
}

//--------------------------------------------------------------------
//	ObjectPick() - after the OccluderPick() is called (optional) the
//	shortened ray can then be used for actual object picking.  The
//	objects in the list are sorted from the closest to the farthest.
//	So getting the first object in the list will be the nearest.
//--------------------------------------------------------------------
//virtual
bool setsPickInterest::ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
{
	return false;
}

//----------------------------------------------------------------------------
// Find the object that matches the pick code from an earlier pick render.
//----------------------------------------------------------------------------
//virtual 
pick3dPickObject* setsPickInterest::MatchPickCode(envType::UInt32 i_PickCode) const
{
	return setsDataMgr::MatchPickCode(i_PickCode);
}


