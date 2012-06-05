/****************************************************************************\
**	chnlTimelinePanel.hpp
**
**		This panel will contain the channels, but will also draw a
**	guideline over the channels, representing the current time.
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMELINEPANEL_HPP
#error chnlTimelinePanel.hpp multiply included
#endif
#define CHNL_TIMELINEPANEL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class chnlTimelinePanel : public wxScrolledWindow
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTimelinePanel(wxWindow* i_pParent);

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &i_Event);

    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
