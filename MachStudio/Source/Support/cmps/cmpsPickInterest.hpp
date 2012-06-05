/*****************************************************************************
**  cmpsPickInterest.hpp
**
**      the pick interest for system cmps.
**
**	StudioGPU
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

		//----------------------------------------------------------------------------
		// Find the object that matches the pick code from an earlier pick render.
		//----------------------------------------------------------------------------
		virtual pick3dPickObject* MatchPickCode(envType::UInt32 i_PickCode) const;
};
