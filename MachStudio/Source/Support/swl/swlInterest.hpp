/****************************************************************************\
**	swlInterest.hpp
**
**		A Software Lighting Interest is registered by a system that has data
**	to be exported to Mray/Rman environmental lighting setting
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#ifdef SWL_INTEREST_HPP
#error swlnterest.hpp multiply included
#endif
#define SWL_INTEREST_HPP

#include <string>
#include <vector>

class swlData;

//============================================================================
//	Forward References
//============================================================================

//============================================================================
//============================================================================
class swlInterest
{
	public:
		//--------------------------------------------------------------------
		// Gather data for objects that will be exported
		//--------------------------------------------------------------------
		virtual void GatherData(const swlData& i_Data ) const = 0;
};
