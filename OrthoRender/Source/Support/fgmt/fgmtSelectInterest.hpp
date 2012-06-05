/*****************************************************************************
**  fgmtSelectInterest.hpp
**
**      the Select interest for fragment flag overriding
**
**	Extra Large Technology
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
		//	SelectFromPickCode - check the pick code of the selected
		//	object and set the selected fragment index to match.
		//--------------------------------------------------------------------
		static bool SelectFromPickCode(pick3dPickObject *i_pPicked,
									   envType::UInt32 i_PickCode);
};
