/*****************************************************************************
**	SelectListDialogUtil.hpp
**
**		API for SelectList dialog
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef SELECTLISTDIALOGUTIL_HPP
#error SelectListDialogUtil.hpp multiply included
#endif
#define SELECTLISTDIALOGUTIL_HPP

#ifndef SEL3D_SELECTINTEREST_HPP
#include "Tool/sel3d/sel3dSelectInterest.hpp"
#endif


//============================================================================
//============================================================================


//============================================================================
//============================================================================
namespace SelectListDialogUtil
{
	class SelectListInterest : public sel3dSelectInterest
	{
		public:
		//--------------------------------------------------------------------
		//	SelectionChanged - called when the selection list is changed
		//--------------------------------------------------------------------
		virtual void SelectionChanged();
	};

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

}	// end of namespace
