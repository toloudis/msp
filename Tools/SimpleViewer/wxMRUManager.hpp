/*****************************************************************************
**  wxMRUManager.hpp
**
**     wxWidgets based class for adding most recently used files to File menu
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WX_MRUMANAGER_HPP
#error wxMRUManager.hpp multiply included
#endif
#define WX_MRUMANAGER_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class wxMRUManager : public wxEvtHandler
{
	public:
		//--------------------------------------------------------------------
		// Makes MRU sub menu for the given item,
		//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
		//	items to the list.
		//--------------------------------------------------------------------
		wxMRUManager(wxMenu* i_pMRUMenu, int i_NumItems, wxMenu* i_pTopLevelMenu);

	private:
		// any class wishing to process wxWidgets events must use this macro
		DECLARE_EVENT_TABLE()

		wxMenu* m_pMRUMenu;
		wxMenu* m_pTopLevelMenu;

		void OnMenuClick(wxCommandEvent& i_Event);
		void OnMenuOpen(wxMenuEvent& i_Event);

};

#endif // USE_WXWIDGETS

