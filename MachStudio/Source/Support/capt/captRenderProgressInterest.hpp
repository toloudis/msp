/****************************************************************************\
**	captRenderProgressInterest.hpp
**
**		An interest related to render progress.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_RENDERPROGRESSINTEREST_HPP
#error captRenderProgressInterest.hpp multiply included
#endif
#define CAPT_RENDERPROGRESSINTEREST_HPP

#ifndef CAPT_RENDERPROGRESSDATA_HPP
#include "Support/capt/captRenderProgressData.hpp"
#endif



//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class captRenderProgressInterest
{
	public:
		//--------------------------------------------------------------------
		//	FrameComplete
		//--------------------------------------------------------------------
		virtual void FrameComplete(const captRenderProgressData& i_Data) = 0;
};

