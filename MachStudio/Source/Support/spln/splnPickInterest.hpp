/*****************************************************************************
**  splnPickInterest.hpp
**
**      the pick interest for system ptlt.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SPLN_PICKINTEREST_HPP
#error splnPickInterest.hpp multiply included
#endif
#define SPLN_PICKINTEREST_HPP

#include "Tool/pick3d/pick3dPickInterest.hpp"


//============================================================================
//============================================================================
class splnPickInterest : public pick3dPickInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		splnPickInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~splnPickInterest();

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const;
};
