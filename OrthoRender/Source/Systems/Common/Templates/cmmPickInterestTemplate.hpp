/*****************************************************************************
**  cmmPickInterestTemplate.hpp
**
**      Pick Interest for systems with lists of objects
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_PICKINTERESTTEMPLATE_HPP
#error cmmPickInterestTemplate.hpp multiply included
#endif
#define CMM_PICKINTERESTTEMPLATE_HPP

#ifndef PICK3D_PICKINTEREST_HPP
#include "Tool/pick3d/pick3dPickInterest.hpp"
#endif

//============================================================================
//============================================================================
template<class xxxObjectMgr>
class cmmPickInterestTemplate : public pick3dPickInterest
{
public:
	//--------------------------------------------------------------------
	//	OccluderPick() - search through objects that are occluders and
	//	modify the tVal so the ray is shortened to only the visible length
	//--------------------------------------------------------------------
	virtual bool OccluderPick(geoPickRay& i_Ray, float &o_tVal)
	{
		return false;
	}

	//--------------------------------------------------------------------
	//	ObjectPick() - after the OccluderPick() is called (optional) the
	//	shortened ray can then be used for actual object picking.  The
	//	objects in the list are sorted from the closest to the farthest.
	//	So getting the first object in the list will be the nearest.
	//--------------------------------------------------------------------
	virtual bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList)
	{
		return xxxObjectMgr::ObjectPick(i_Ray, io_PickList);
	}

	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const
	{
		return xxxObjectMgr::MatchPickCode(i_PickCode);
	}
};
