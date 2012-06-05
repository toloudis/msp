/*****************************************************************************
**	grupGroupObjectsPage.hpp
**
**	Group Objects tab page in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef GRUP_GROUPOBJECTSPAGE_HPP
#error grupGroupObjectsPage.hpp multiply included
#endif
#define GRUP_GROUPOBJECTSPAGE_HPP

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
#include "Systems/Groups/GUI/wxGUI/grupGroupObjectsPageBase.h"


//----------------------------------------------------------------------------
// Class grupGroupObjectsPage
//----------------------------------------------------------------------------
class grupGroupObjectsPage : public grupGroupObjectsPageBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static grupGroupObjectsPage* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		grupGroupObjectsPage( wxWindow* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~grupGroupObjectsPage();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------	
		void Update(const nameString& i_GroupName);
		void ReUpdate() { Update(m_GroupName); }

	private:

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void checkList_Objects_Toggle( wxCommandEvent& i_Event);
	
		nameString m_GroupName;
};

#endif // USE_WXWIDGETS
