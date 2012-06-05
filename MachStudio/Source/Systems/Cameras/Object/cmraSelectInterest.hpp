/*****************************************************************************
**  cmraSelectInterest.hpp
**
**      the Select interest for system cameras.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef CMRA_SELECTINTEREST_HPP
#error cmraSelectInterest.hpp multiply included
#endif
#define CMRA_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


//============================================================================
//============================================================================
class cmraSelectInterest : public sel3dSelectInterest
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
		virtual void AddedToSelection( sel3dObject* i_pSelObj );

		//--------------------------------------------------------------------
		//	RemovedFromSelection - called when an object is removed from
		//		the selection list.
		//--------------------------------------------------------------------
		virtual void RemovedFromSelection( sel3dObject* i_pSelObj );
};
