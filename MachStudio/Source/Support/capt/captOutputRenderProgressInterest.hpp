/****************************************************************************\
**	captRenderProgressInterest.hpp
**
**		An interest related to render progress.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_OUTPUTRENDERPROGRESSINTEREST_HPP
#error captOutputRenderProgressInterest.hpp multiply included
#endif
#define CAPT_OUTPUTRENDERPROGRESSINTEREST_HPP

#ifndef CAPT_RENDERPROGRESSINTEREST_HPP
#include "Support/capt/captRenderProgressInterest.hpp"
#endif



//============================================================================
//	Forward References
//============================================================================


//============================================================================
//============================================================================
class captOutputRenderProgressInterest : public captRenderProgressInterest
{
	public:
		//--------------------------------------------------------------------
		//	FrameComplete
		//--------------------------------------------------------------------
		virtual void FrameComplete(const captRenderProgressData& i_Data);
};

