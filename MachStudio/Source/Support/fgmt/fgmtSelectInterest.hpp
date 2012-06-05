/*****************************************************************************
**  fgmtSelectInterest.hpp
**
**      the Select interest for fragment flag overriding
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_SELECTINTEREST_HPP
#error fgmtSelectInterest.hpp multiply included
#endif
#define FGMT_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif

//============================================================================
//============================================================================
class fgmtSelectInterest : public sel3dSelectInterest
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
					
		//--------------------------------------------------------------------
		// Returns the selectable object for the surface part selected
		// with the GPU pick code given. Returns NULL if no fragment selected.
		//--------------------------------------------------------------------
		static sel3dObject* GetPartFromPickCode(sel3dObject *i_pPicked,
												envType::UInt32 i_PickCode);
};
