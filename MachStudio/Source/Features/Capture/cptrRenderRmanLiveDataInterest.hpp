/****************************************************************************\
**	cptrRenderRmanLiveDataInterest.hpp
**
**		An interest related to preferences.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERRMANLIVEDATAINTEREST_HPP
#error cptrRenderRmanLiveDataInterest.hpp multiply included
#endif
#define CPTR_RENDERRMANLIVEDATAINTEREST_HPP

#ifndef CPTR_RENDERRMANLIVEDATA_HPP
#include "Features/Capture/cptrRenderRmanLiveData.hpp"
#endif



//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class cptrRenderRmanLiveDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	PrefsDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void RenderRmanLiveDataUpdated(const cptrRenderRmanLiveData& i_Data) = 0;
};

