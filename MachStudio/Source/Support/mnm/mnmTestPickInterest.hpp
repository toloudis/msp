/*****************************************************************************
**  mnmTestPickInterest.hpp
**
**      the pick interest for system mnmTest.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef MNM_TESTPICKINTEREST_HPP
#error mnmTestPickInterest.hpp multiply included
#endif
#define MNM_TESTPICKINTEREST_HPP

#include "pick3dPickInterest.hpp"


//============================================================================
//============================================================================
class mnmTestPickInterest : public pick3dPickInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		mnmTestPickInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~mnmTestPickInterest();

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
};
