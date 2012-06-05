/****************************************************************************\
**	visVisibleInterest.hpp
**
**		A Visible Interest is usually related to a system that cares about
**	objects being visible or not.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef VIS_SELECTINTEREST_HPP
#error visVisibleInterest.hpp multiply included
#endif
#define VIS_SELECTINTEREST_HPP

#include <string>


//============================================================================
//	Forward References
//============================================================================

//============================================================================
//============================================================================
class visVisibleInterest
{
	public:
		//--------------------------------------------------------------------
		//	ShowIcons - show or hide icons that are not part of real scene.
		//--------------------------------------------------------------------
		virtual void ShowIcons( bool i_bVisible ) = 0;

		//--------------------------------------------------------------------
		// Make sure that all geometry is visible for rendering
		//--------------------------------------------------------------------
		virtual void ConfirmGeometryVisible();

		//--------------------------------------------------------------------
		// Set object visible state while editting
		//--------------------------------------------------------------------
		virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible) = 0;
};
