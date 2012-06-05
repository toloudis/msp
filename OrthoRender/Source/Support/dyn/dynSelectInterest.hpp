/*****************************************************************************
**  dynSelectInterest.hpp
**
**      the Select interest for system mnmTest.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DYN_SELECTINTEREST_HPP
#error dynSelectInterest.hpp multiply included
#endif
#define DYN_SELECTINTEREST_HPP

#include "Tool/sel3d/sel3dSelectInterest.hpp"


//============================================================================
//============================================================================
class dynSelectInterest : public sel3dSelectInterest
{
	public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//--------------------------------------------------------------------
		virtual void SelectionChanged();
};
