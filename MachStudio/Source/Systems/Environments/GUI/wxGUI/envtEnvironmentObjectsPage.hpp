/*****************************************************************************
**	envtEnvironmentObjectsPage.hpp
**
**	Environment Objects tab page in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTOBJECTSPAGE_HPP
#error envtEnvironmentObjectsPage.hpp multiply included
#endif
#define ENVT_ENVIRONMENTOBJECTSPAGE_HPP

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
#include "Systems/Environments/GUI/wxGUI/envtEnvironmentObjectsPageBase.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class envtScriptObject;
class nameString;

//----------------------------------------------------------------------------
// Class envtEnvironmentObjectsPage
//----------------------------------------------------------------------------
class envtEnvironmentObjectsPage : public envtEnvironmentObjectsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static envtEnvironmentObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtEnvironmentObjectsPage( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~envtEnvironmentObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------	
		void Update(const nameString& i_EnvironmentName);
		void ReUpdate() { Update(m_EnvironmentName); }

	private:

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void checkList_Objects_Toggle( wxCommandEvent& i_Event);
	
		nameString m_EnvironmentName;
};

#endif // USE_WXWIDGETS
