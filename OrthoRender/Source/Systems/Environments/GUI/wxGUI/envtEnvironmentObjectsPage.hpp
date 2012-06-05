/*****************************************************************************
**	envtEnvironmentObjectsPage.hpp
**
**	Environment Objects tab page in wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTOBJECTSPAGE_HPP
#error envtEnvironmentObjectsPage.hpp multiply included
#endif
#define ENVT_ENVIRONMENTOBJECTSPAGE_HPP

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
		void Update(envtScriptObject *i_pScriptObject);

	private:

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void checkList_Objects_Toggle( wxCommandEvent& i_Event);
	
		envtScriptObject *m_pObject;
};

#endif // USE_WXWIDGETS
