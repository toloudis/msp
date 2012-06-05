/*****************************************************************************
**  setsVisibleInterest.hpp
**
**      the visible interest for system prop.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef SETS_VISIBLEINTEREST_HPP
#error setsVisibleInterest.hpp multiply included
#endif
#define SETS_VISIBLEINTEREST_HPP

#include "Support/vis/visVisibleInterest.hpp"


//============================================================================
//============================================================================
class setsVisibleInterest : public visVisibleInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		setsVisibleInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~setsVisibleInterest();

		//--------------------------------------------------------------------
		//	ShowIcons - show or hide icons that are not part of real scene.
		//--------------------------------------------------------------------
		virtual void ShowIcons( bool i_bVisible );

		//--------------------------------------------------------------------
		// Make sure that all geometry is visible for rendering
		//--------------------------------------------------------------------
		virtual void ConfirmGeometryVisible();

		//--------------------------------------------------------------------
		// Set object visible state while editting
		//--------------------------------------------------------------------
		virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible);
};
