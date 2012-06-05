/*****************************************************************************
**  wxRenderCanvas.hpp
**
**     Window using wxWidgets for doing Terawatt rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WX_RENDERCANVAS_HPP
#error wxRenderCanvas.hpp multiply included
#endif
#define WX_RENDERCANVAS_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
// Widget to handle rendering, resizes window when panel is resized
//============================================================================
class wxRenderCanvas : public wxPanel
{
public:
    // ctor(s)
    wxRenderCanvas(wxWindow* parent);

    // event handlers (these functions should _not_ be virtual)
    //void OnPaint(wxPaintEvent &i_Event);
	void OnMouseMove(wxMouseEvent& i_Event);
	void OnEnter(wxMouseEvent& i_Event);
	void OnLeave(wxMouseEvent& i_Event);
	void OnSize(wxSizeEvent& i_Event);

private:
    DECLARE_EVENT_TABLE()
};

#endif // USE_WXWIDGETS
