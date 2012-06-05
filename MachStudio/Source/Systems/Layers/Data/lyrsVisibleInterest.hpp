/*****************************************************************************
**  lyrsVisibleInterest.hpp
**
**      the visible interest for system prop.
**
**	StudioGPU
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
		// Set object active state in the render layer
		//--------------------------------------------------------------------
		virtual void SetActiveInRenderLayer(std::string& i_ObjectName, bool i_bActive)
		{};

		//--------------------------------------------------------------------
		// Set object visible state while editting
		//--------------------------------------------------------------------
		virtual void SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible)
		{
		};

		//--------------------------------------------------------------------
		// Set object visible state while editting
		//--------------------------------------------------------------------
		virtual bool GetVisibleInEditor(std::string& i_ObjectName)
		{
			return false;
		};

		//--------------------------------------------------------------------
		// Get whether or not this interest has the object
		//--------------------------------------------------------------------
		virtual bool InterestHasObject(std::string& i_ObjectName)
		{
			return false;
		};
};
