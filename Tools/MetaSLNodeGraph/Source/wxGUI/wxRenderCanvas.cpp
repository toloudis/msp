/*****************************************************************************
**  wxRenderCanvas.hpp
**
**     Window using wxWidgets for doing Terawatt rendering
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "wxRenderCanvas.hpp"
#include "mspApp.hpp"

#include "Tool/tma3d/tma3dCursorMgr.hpp"

#ifdef USE_WXWIDGETS

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them.
BEGIN_EVENT_TABLE(wxRenderCanvas, wxPanel)
//    EVT_PAINT  (wxRenderCanvas::OnPaint)
	EVT_MOTION(wxRenderCanvas::OnMouseMove)
	EVT_ENTER_WINDOW(wxRenderCanvas::OnEnter)
	EVT_LEAVE_WINDOW(wxRenderCanvas::OnLeave)
    EVT_SIZE(wxRenderCanvas::OnSize)
END_EVENT_TABLE()


//--------------------------------------------------------------------
// canvas constructor
//--------------------------------------------------------------------
wxRenderCanvas::wxRenderCanvas(wxWindow* parent)
	: wxPanel(parent, wxID_ANY)
{
	this->SetBackgroundColour( *wxBLACK );
}

//
////--------------------------------------------------------------------
//// paint event handler
////--------------------------------------------------------------------
//void wxRenderCanvas::OnPaint(wxPaintEvent& WXUNUSED(i_Event))
//{
//    wxPaintDC pdc(this);
//
//    PrepareDC(pdc);
//
//	// Draw lines to see if this works
//    pdc.SetPen(*wxMEDIUM_GREY_PEN);
//    for ( int i = 0; i < 200; i++ )
//        pdc.DrawLine(0, i*10, i*10, 0);
//}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxRenderCanvas::OnMouseMove(wxMouseEvent& i_Event)
{	
	tma3dCursorMgr::CursorPosChangedCallback( i_Event.GetX(), i_Event.GetY() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxRenderCanvas::OnEnter(wxMouseEvent& i_Event)
{	
	tma3dCursorMgr::CursorOverViewCallback(true);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxRenderCanvas::OnLeave(wxMouseEvent& i_Event)
{	
	tma3dCursorMgr::CursorOverViewCallback(false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void wxRenderCanvas::OnSize(wxSizeEvent& i_Event)
{		
	wxRect rect = this->GetClientRect();
//	DBG_LOG2("Render window size: %d %d", rect.GetWidth(), rect.GetHeight());
	
	mspApp::ResizeRender(rect.GetWidth(), rect.GetHeight());
}

#endif // USE_WXWIDGETS