/*****************************************************************************
**  pntPickInterest.hpp
**
**      the pick interest for system ptlt.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef PNT_PICKINTEREST_HPP
#error pntPickInterest.hpp multiply included
#endif
#define PNT_PICKINTEREST_HPP

#include "Tool/pick3d/pick3dPickInterest.hpp"


//============================================================================
//============================================================================
class pntPickInterest : public pick3dPickInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		pntPickInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~pntPickInterest();

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const;
};
