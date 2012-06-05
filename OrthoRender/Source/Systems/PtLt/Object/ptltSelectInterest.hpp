/*****************************************************************************
**  ptltSelectInterest.hpp
**
**      the Select interest for system point lights.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef PTLT_SELECTINTEREST_HPP
#error ptltSelectInterest.hpp multiply included
#endif
#define PTLT_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


//============================================================================
//============================================================================
class ptltSelectInterest : public sel3dSelectInterest
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
