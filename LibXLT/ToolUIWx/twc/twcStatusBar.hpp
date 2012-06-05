/****************************************************************************\
**	twcStatusBar.hpp
**
**		Status bar that monitors mouse motion in order to set the tool
**	tip based on over which panel of the status bar the mouse is hovering.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_STATUSBAR_HPP
#error twcStatusBar.hpp multiply included
#endif
#define TWC_STATUSBAR_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <vector>
#include <string>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class twcStatusBar : public wxStatusBar
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcStatusBar(wxWindow* i_pParent, 
					   wxWindowID i_Id = wxID_ANY,
					   long i_Style = wxST_SIZEGRIP,
					   const wxString& i_Name = L"statusBar");

		//--------------------------------------------------------------------
		// Set the tool tip for a given panel by index
		//--------------------------------------------------------------------
		void SetPanelToolTip(int i_Panel, const std::string& i_String);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnMouseMove(wxMouseEvent &i_Event);

		std::vector<std::string> m_ToolTips;

		DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
