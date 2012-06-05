/*****************************************************************************
**  chnlSelectInterest.hpp
**
**      the Select interest for the channel editor package.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CHNL_SELECTINTEREST_HPP
#error chnlSelectInterest.hpp multiply included
#endif
#define CHNL_SELECTINTEREST_HPP

#include "Tool/sel3d/sel3dSelectInterest.hpp"


//============================================================================
//============================================================================
class chnlSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//	at all. Many interests may only need to override this function.
		//--------------------------------------------------------------------
		virtual void SelectionChanged();
};
