/*****************************************************************************
**  dcutSelectInterest.hpp
**
**      the Select interest for system director's cut.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef DCUT_SELECTINTEREST_HPP
#error dcutSelectInterest.hpp multiply included
#endif
#define DCUT_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


//============================================================================
//============================================================================
class dcutSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//--------------------------------------------------------------------
		virtual void SelectionChanged();
};
