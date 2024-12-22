/****************************************************************************\
**	chnlTimeSlider.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimeSlider.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"

#include <sstream>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	std::string convert_num(int i_Num)
	{
		std::ostringstream str;
		str << i_Num;
		return str.str();
	}

	void draw_num(wxDC &io_DC, int i_Num, int i_X, int i_Y)
	{
		wxString text(convert_num(i_Num).c_str(), wxConvUTF8);

		// center the numbers
		wxSize text_size = io_DC.GetTextExtent(text);
		float center_off = text_size.GetWidth() * 0.5f;
		io_DC.DrawText(text, i_X-center_off, i_Y); 
	}
}


//--------------------------------------------------------------------
// event table
//--------------------------------------------------------------------
BEGIN_EVENT_TABLE(chnlTimeSlider, wxControl)
    EVT_PAINT(chnlTimeSlider::OnPaint)
	EVT_LEFT_DOWN(chnlTimeSlider::OnMouseDown)
	EVT_LEFT_UP(chnlTimeSlider::OnMouseUp)
    EVT_MOTION(chnlTimeSlider::OnMouseMove)
	EVT_MOUSE_CAPTURE_CHANGED(chnlTimeSlider::OnCaptureChanged)
	EVT_MOUSE_CAPTURE_LOST(chnlTimeSlider::OnCaptureLost)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTimeSlider::chnlTimeSlider(wxWindow* i_pParent)
:	wxControl(i_pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE),
	m_CurrentTime(0),
	m_bScrubbing(false)
{	
	// Since we are drawing the background ourself, we can set the custom flag
	// to prevent flickering:
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	this->compute_size();	
}

//--------------------------------------------------------------------
// CurrentTime
//--------------------------------------------------------------------
void chnlTimeSlider::SetCurrentTime(float i_Time)
{
	m_CurrentTime = i_Time;
	this->Refresh();
}
float chnlTimeSlider::GetCurrentTime() const
{
	return m_CurrentTime;
}

//--------------------------------------------------------------------
// Get the horizontal position of the current time
//--------------------------------------------------------------------
int chnlTimeSlider::GetCurrentTimePosition()
{
	int pos = get_position_for_time(m_CurrentTime);
	return pos;
}

//--------------------------------------------------------------------
//	Get the time based on the horizontal position
//--------------------------------------------------------------------
float chnlTimeSlider::GetTimeAtPosition(int i_ScreenXPos)
{
	float time = get_time_at_position(i_ScreenXPos);
	return time;
}

//----------------------------------------------------------------------------
// Clear time ticks
//----------------------------------------------------------------------------
void chnlTimeSlider::ClearTimeTicks()
{
	m_TimeTicks.clear();
	this->Refresh();
}

//----------------------------------------------------------------------------
// Add a time tick
//----------------------------------------------------------------------------
void chnlTimeSlider::AddTimeTick(float i_Time)
{
	m_TimeTicks.insert(i_Time);
	this->Refresh();
}

//--------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//--------------------------------------------------------------------
void chnlTimeSlider::update_size()
{
	this->compute_size();
	this->Refresh();
}

//----------------------------------------------------------------------------
// Adjust size of control based on expanded and categories of clips
//----------------------------------------------------------------------------
void chnlTimeSlider::compute_size()
{
	wxSize size(this->get_full_width(), 16);
	this->SetMinSize(size);
	this->SetSize(size);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC pdc(this);

//#if wxUSE_GRAPHICS_CONTEXT
//     wxGCDC gdc( pdc ) ;
//    wxDC &dc = m_useContext ? (wxDC&) gdc : (wxDC&) pdc ;
//#else
    wxDC &dc = pdc ;
//#endif

    PrepareDC(dc);

    dc.Clear();

	wxSize size = this->GetSize();
	int ht = size.GetHeight() - 4;

	// Outline the client rectangle (instead of using a windows border)
	dc.SetPen( wxPen( *wxLIGHT_GREY, 1, wxPENSTYLE_SOLID ) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
	dc.DrawRectangle(0, 0, size.GetWidth(), size.GetHeight());

	// Draw time ticks
	wxPen tick_pen( *wxLIGHT_GREY, 1, wxPENSTYLE_SOLID);
	dc.SetPen(tick_pen);
	std::set<float>::const_iterator it;
	for (it = m_TimeTicks.begin(); it != m_TimeTicks.end(); ++it)
	{
		float time(*it);
		int pos = get_position_for_time(time);
		dc.DrawLine( pos, 0, pos, ht);
	}
	

	// Draw color triangle at current time
	int pos = get_position_for_time(m_CurrentTime);
	int off = ht / 2;

	wxPoint points[3];
	points[0].x = pos; points[0].y = 0;
	points[1].x = pos-off; points[1].y = ht;
	points[2].x = pos+off; points[2].y = ht;

	// Fill color is firebrick red
	dc.SetPen( wxPen(*wxBLACK, 1, wxPENSTYLE_SOLID) );
	dc.SetBrush(wxBrush(wxColour(178, 34, 34), wxBRUSHSTYLE_SOLID));
	dc.DrawPolygon(3, points);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::OnMouseDown(wxMouseEvent &i_Event)
{
	if (i_Event.LeftDown())
	{
		this->CaptureMouse();
		m_bScrubbing = true;
		tmlnTimeLine::SetIsScrubbing( true );

		handle_mouse_event(i_Event);
	}

	i_Event.Skip();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::OnMouseUp(wxMouseEvent &i_Event)
{
	if (i_Event.LeftUp())
	{
		m_bScrubbing = false;
		tmlnTimeLine::SetIsScrubbing( false );
		if(this->HasCapture())
			this->ReleaseMouse();
	}
	i_Event.Skip();
}

//----------------------------------------------------------------------------
// Called when capture is lost because of reason besides mouse up
//----------------------------------------------------------------------------
void chnlTimeSlider::OnCaptureChanged(wxMouseCaptureChangedEvent &i_Event)
{
	m_bScrubbing = false;
	tmlnTimeLine::SetIsScrubbing( false );
	if(this->HasCapture())
		this->ReleaseMouse();
}
void chnlTimeSlider::OnCaptureLost(wxMouseCaptureLostEvent &i_Event)
{
	m_bScrubbing = false;
	tmlnTimeLine::SetIsScrubbing( false );
	if(this->HasCapture())
		this->ReleaseMouse();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::OnMouseMove(wxMouseEvent &i_Event)
{
	// m_bScrubbing is true if the mouse down event 
	// was on our control. We don't want to alter the timeline
	// when the user is controlling the camera from the render window and
	// moves over our control.
	if (m_bScrubbing)
		handle_mouse_event(i_Event);

	i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::handle_mouse_event(wxMouseEvent &i_Event)
{
	if (i_Event.LeftIsDown())
	{
		float time = get_time_at_position(i_Event.GetX());
		maFunctions::Clamp(time, m_MinTime, m_MaxTime); 
		tmlnTimeUtil::AdjustTimeToFrame( time );
		if (m_CurrentTime != time)
		{
			m_CurrentTime = time;
			this->Refresh();
			
			// trigger the callback
			wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
			this->ProcessCommand(changed_event);
		}
	}
}

#endif // USE_WXWIDGETS
