/****************************************************************************\
**	twcPictureBox.hpp
**
**		Custom control with a panel that could apply texture on
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_PICTUREBOX_HPP
#error twcPictureBox.hpp multiply included
#endif
#define TWC_PICTUREBOX_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#include <wx/panel.h>
#include <wx/sizer.h>

#ifdef USE_WXWIDGETS

//============================================================================
// wxEVT_VALUE_CHANGED event : this control throws this event type when the
//	value in the control has changed aftere ENTER is pressed, or the
//	focus leaves the text box. Users should register for this event, 
//	not for the individual enter pressed and leave events.
//============================================================================

//============================================================================
//============================================================================
class twcPictureBox : public wxPanel
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcPictureBox(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxNO_BORDER,
            const wxString& name = wxPanelNameStr);
		~twcPictureBox();

		
		void LoadImage(wxString i_file, bool i_changePanelSize = false, wxBitmapType in_format = wxBITMAP_TYPE_BMP);
		void SetImage(wxBitmap* i_tex, bool i_changePanelSize = false);
		void paintEvent(wxPaintEvent & evt);
        void paintNow();

	private:
		void render(wxDC& i_dc);

		wxBitmap* m_texture;
		DECLARE_EVENT_TABLE()

};

#endif // USE_WXWIDGETS
