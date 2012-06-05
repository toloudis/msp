/*****************************************************************************
**	captRenderProgressMgr.hpp
**
**		
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef CAPT_RENDERPROGRESSMGR_HPP
#error captRenderProgressMgr.hpp multiply included
#endif
#define CAPT_RENDERPROGRESSMGR_HPP

#ifndef CAPT_RENDERPROGRESSDATA_HPP
#include "Support/capt/captRenderProgressData.hpp"
#endif

//============================================================================
//	Forward References
//============================================================================

class captRenderProgressInterest;
//============================================================================
//============================================================================
namespace captRenderProgressMgr
{
	//------------------------------------------------------------------------
	//  CleanUp
	//------------------------------------------------------------------------
	void  CleanUp();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	captRenderProgressData& Data();

	//
	//	interest functions
	//

	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( captRenderProgressInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( captRenderProgressInterest* i_pInterest );

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void ClearInterests();

	//------------------------------------------------------------------------
	//	This gets called to let the interests know that the data has changed
	//------------------------------------------------------------------------
	void FrameComplete();
}
