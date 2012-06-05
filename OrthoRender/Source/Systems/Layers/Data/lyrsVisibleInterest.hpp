/*****************************************************************************
**  lyrsVisibleInterest.hpp
**
**      the visible interest for system prop.
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef LYRS_VISIBLEINTEREST_HPP
#error lyrsVisibleInterest.hpp multiply included
#endif
#define LYRS_VISIBLEINTEREST_HPP

#include "Support/vis/visVisibleInterest.hpp"


//============================================================================
//============================================================================
class lyrsVisibleInterest : public visVisibleInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lyrsVisibleInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~lyrsVisibleInterest();

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
		virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible)
		{
		};
};
