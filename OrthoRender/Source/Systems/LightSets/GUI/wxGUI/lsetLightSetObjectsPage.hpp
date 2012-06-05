/*****************************************************************************
**	lsetLightSetObjectsPage.hpp
**
**	LightSet Lights tab page in wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETOBJECTSPAGE_HPP
#error lsetLightSetObjectsPage.hpp multiply included
#endif
#define LSET_LIGHTSETOBJECTSPAGE_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetObjectsPageBase.h"


//----------------------------------------------------------------------------
// Class lsetLightSetObjectsPage
//----------------------------------------------------------------------------
class lsetLightSetObjectsPage : public lsetLightSetObjectsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lsetLightSetObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetObjectsPage( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lsetLightSetObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------	
		void Update(const nameString& i_LightSetName);

	private:
		//--------------------------------------------------------------------
		// Item was clicked on, toggle its checked state
		//--------------------------------------------------------------------
		void toggle_checked_state(wxTreeItemId i_Item);

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void treeCtrl_Objects_LeftMouseDown( wxMouseEvent& i_Event);
	
		nameString m_LightSetName;
};

#endif // USE_WXWIDGETS
