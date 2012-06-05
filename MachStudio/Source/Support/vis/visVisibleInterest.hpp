/****************************************************************************\
**	visVisibleInterest.hpp
**
**		A Visible Interest is usually related to a system that cares about
**	objects being visible or not.
**
**	StudioGPU
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
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		virtual void SetActiveInRenderLayer(std::string& i_ObjectName, bool i_bActive) = 0;

		//--------------------------------------------------------------------
		// Set object visible state while editting
		//--------------------------------------------------------------------
		virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible) = 0;

		//--------------------------------------------------------------------
		// Get object visible state while editting
		//--------------------------------------------------------------------
		virtual bool GetVisibleInEditor(std::string& i_ObjectName) = 0;

		//--------------------------------------------------------------------
		// Get whether or not this interest has the object
		//--------------------------------------------------------------------
		virtual bool InterestHasObject(std::string& i_ObjectName) = 0;
};
