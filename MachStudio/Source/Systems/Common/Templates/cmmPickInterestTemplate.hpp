/*****************************************************************************
**  cmmPickInterestTemplate.hpp
**
**      Pick Interest for systems with lists of objects
**
**	StudioGPU
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
	//----------------------------------------------------------------------------
	// Find the object that matches the pick code from an earlier pick render.
	//----------------------------------------------------------------------------
	virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const
	{
		return xxxObjectMgr::MatchPickCode(i_PickCode);
	}
};
