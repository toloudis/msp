/*****************************************************************************
**  mtrlSelectInterest.hpp
**
**      the Select interest for material overriding
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_SELECTINTEREST_HPP
#error mtrlSelectInterest.hpp multiply included
#endif
#define MTRL_SELECTINTEREST_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif
#ifndef ENV_TYPE_HPP
#include "Core/env/envType.hpp"
#endif



//============================================================================
//============================================================================
class mtrlSelectInterest : public sel3dSelectInterest
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
		// Returns the selectable object for the material part selected
		// with the GPU pick code given. Returns NULL if no material selected.
		//--------------------------------------------------------------------
		static sel3dObject* GetPartFromPickCode(sel3dObject *i_pPicked,
												envType::UInt32 i_PickCode);

		//--------------------------------------------------------------------
		//	SelectFromPickCode - check the pick code of the selected
		//	object and set the selected material index to match.
		//--------------------------------------------------------------------
		static bool SelectFromPickCode(sel3dObject *i_pPicked,
										envType::UInt32 i_PickCode,
										bool i_bAppendSelection);
};
