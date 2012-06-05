/*****************************************************************************
**  propSelectInterest.hpp
**
**      the Select interest for the props system.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef PROP_SELECTINTEREST_HPP
#error propSelectInterest.hpp multiply included
#endif
#define PROP_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


//============================================================================
//============================================================================
class propSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//--------------------------------------------------------------------
		virtual void SelectionChanged();

		//--------------------------------------------------------------------
		//	AddedToSelection - called when an object is added to the
		//		selection list.
		//--------------------------------------------------------------------
		virtual void AddedToSelection( pick3dPickObject* i_pSelObj );

		//--------------------------------------------------------------------
		//	RemovedFromSelection - called when an object is removed from
		//		the selection list.
		//--------------------------------------------------------------------
		virtual void RemovedFromSelection( pick3dPickObject* i_pSelObj );
};
