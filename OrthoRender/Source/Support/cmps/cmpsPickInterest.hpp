/*****************************************************************************
**  cmpsPickInterest.hpp
**
**      the pick interest for system cmps.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMPS_PICKINTEREST_HPP
#error cmpsPickInterest.hpp multiply included
#endif
#define CMPS_PICKINTEREST_HPP

#include "Tool/pick3d/pick3dPickInterest.hpp"


//============================================================================
//============================================================================
class cmpsPickInterest : public pick3dPickInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmpsPickInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~cmpsPickInterest();

		//--------------------------------------------------------------------
		//	OccluderPick() - search through objects that are occluders and
		//	modify the tVal so the ray is shortened to only the visible length
		//--------------------------------------------------------------------
		virtual bool OccluderPick(geoPickRay& i_Ray, float &o_tVal);

		//--------------------------------------------------------------------
		//	ObjectPick() - after the OccluderPick() is called (optional) the
		//	shortened ray can then be used for actual object picking.  The
		//	objects in the list are sorted from the closest to the farthest.
		//	So getting the first object in the list will be the nearest.
		//--------------------------------------------------------------------
		virtual bool ObjectPick(geoPickRay& i_Ray, pick3dPickList& io_PickList);

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const;
};
