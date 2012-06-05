/*****************************************************************************
**  mtrlSelectInterest.hpp
**
**      the Select interest for material overriding
**
**	Extra Large Technology
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
		//	SelectFromPickCode - check the pick code of the selected
		//	object and set the selected material index to match.
		//--------------------------------------------------------------------
		static bool SelectFromPickCode(pick3dPickObject *i_pPicked,
										envType::UInt32 i_PickCode);
};
