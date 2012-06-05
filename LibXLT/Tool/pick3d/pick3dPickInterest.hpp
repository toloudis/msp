/****************************************************************************\
**	pick3dPickInterest.hpp
**
**		A Pick Interest is usually related to a system that cares about
**	objects being picked or occluded in it.
**
**		Usually the occluder function is called to shorten the ray to the
**	visible area and then the object function is called to produce a list
**	of picked objects.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PICK3D_PICKINTEREST_HPP
#error pick3dPickInterest.hpp multiply included
#endif
#define PICK3D_PICKINTEREST_HPP

#ifndef PICK3D_PICKITEM_HPP
#include "Tool/pick3d/pick3dPickItem.hpp"
#endif

#ifndef PICK3D_PICKLIST_HPP
#include "Tool/pick3d/pick3dPickList.hpp"
#endif

#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

#include <vector>


//============================================================================
//	Forward References
//============================================================================
class geoPickRay;


//============================================================================
//============================================================================
class pick3dPickInterest
{
	public:
		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const = 0;
};
