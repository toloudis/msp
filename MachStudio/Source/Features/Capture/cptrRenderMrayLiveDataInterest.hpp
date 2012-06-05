/****************************************************************************\
**	cptrRenderMrayLiveDataInterest.hpp
**
**		An interest related to preferences.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CPTR_RENDERMRAYLIVEDATAINTEREST_HPP
#error cptrRenderMrayLiveDataInterest.hpp multiply included
#endif
#define CPTR_RENDERMRAYLIVEDATAINTEREST_HPP

#ifndef CPTR_RENDERMRAYLIVEDATA_HPP
#include "Features/Capture/cptrRenderMrayLiveData.hpp"
#endif



//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class cptrRenderMrayLiveDataInterest
{
	public:
		//--------------------------------------------------------------------
		//	PrefsDataUpdated - the data was updated
		//--------------------------------------------------------------------
		virtual void RenderMrayLiveDataUpdated(const cptrRenderMrayLiveData& i_Data) = 0;
};

