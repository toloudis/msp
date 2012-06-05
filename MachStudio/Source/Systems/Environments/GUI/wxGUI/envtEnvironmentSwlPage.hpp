/*****************************************************************************
**	envtEnvironmentSwlPage.hpp
**
**	Environment Software lighting tab page in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef ENVT_ENVIRONMENTSWLPAGE_HPP
#error envtEnvironmentSwlPage.hpp multiply included
#endif
#define ENVT_ENVIRONMENTSWLPAGE_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif 
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
#include <wx/panel.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class swlScriptObject;
class nameString;

//----------------------------------------------------------------------------
// Class envtEnvironmentSwlPage
//----------------------------------------------------------------------------
class envtEnvironmentSwlPage : public wxPanel 
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static envtEnvironmentSwlPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		envtEnvironmentSwlPage( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~envtEnvironmentSwlPage();

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
		wxScrolledWindow* m_tabPage_SoftwareLighting;

		nameString m_EnvironmentName;
};

#endif // USE_WXWIDGETS
