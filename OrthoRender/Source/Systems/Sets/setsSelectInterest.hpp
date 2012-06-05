/*****************************************************************************
**  setsSelectInterest.hpp
**
**      the Select interest for system sets.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef SETS_SELECTINTEREST_HPP
#error setsSelectInterest.hpp multiply included
#endif
#define SETS_SELECTINTEREST_HPP

#include "Tool/sel3d/sel3dSelectInterest.hpp"


//============================================================================
//============================================================================
class setsSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//--------------------------------------------------------------------
		virtual void SelectionChanged();
};
