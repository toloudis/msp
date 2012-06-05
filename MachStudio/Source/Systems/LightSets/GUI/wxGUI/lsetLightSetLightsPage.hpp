/*****************************************************************************
**	lsetLightSetLightsPage.hpp
**
**	LightSet Lights tab page in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef LSET_LIGHTSETLIGHTSPAGE_HPP
#error lsetLightSetLightsPage.hpp multiply included
#endif
#define LSET_LIGHTSETLIGHTSPAGE_HPP

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
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetLightsPageBase.h"


//----------------------------------------------------------------------------
// Class lsetLightSetLightsPage
//----------------------------------------------------------------------------
class lsetLightSetLightsPage : public lsetLightSetLightsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static lsetLightSetLightsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		lsetLightSetLightsPage( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~lsetLightSetLightsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------	
		void Update(const nameString& i_LightSetName);
		void ReUpdate() { Update(m_LightSetName); }

	private:

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void checkList_Lights_Toggle( wxCommandEvent& i_Event);
	
		nameString m_LightSetName;
};

#endif // USE_WXWIDGETS
