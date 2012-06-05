/****************************************************************************\
**	tqcStatusBar.hpp
**
**		Status bar that monitors mouse motion in order to set the tool
**	tip based on over which panel of the status bar the mouse is hovering.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_STATUSBAR_HPP
#error tqcStatusBar.hpp multiply included
#endif
#define TQC_STATUSBAR_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>
#include <string>


#ifdef QT_FINISH_PORT

//============================================================================
//============================================================================
class tqcStatusBar : public wxStatusBar
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcStatusBar(QWidget* i_pParent, 
					   QWidgetID i_Id = wxID_ANY,
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

#endif // USE_QT
