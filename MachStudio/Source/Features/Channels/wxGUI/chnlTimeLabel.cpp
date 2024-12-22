/****************************************************************************\
**	chnlTimeLabel.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTimeLabel.hpp"

#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Ma/maTime.hpp"

#include <sstream>
#include <iomanip>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string convert_num(int i_Num, bool i_bDisplayMinutes)
	{
		std::ostringstream str;
		if (i_bDisplayMinutes)
		{
			str << std::setw(2) << std::setfill('0') << (i_Num / 60) << ":" 
				<< std::setw(2) << std::setfill('0') << (i_Num % 60);
		}
		else
			str << i_Num;
		return str.str();
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void draw_num(wxDC &io_DC, int i_Num, int i_X, int i_Y, bool i_bDisplayMinutes)
	{
		wxString text(convert_num(i_Num, i_bDisplayMinutes).c_str(), wxConvUTF8);

		// center the numbers
		wxSize text_size = io_DC.GetTextExtent(text);
		float center_off = text_size.GetWidth() * 0.5f;
		io_DC.DrawText(text, i_X-center_off, i_Y); 
	}
}


//--------------------------------------------------------------------
// event table
//--------------------------------------------------------------------
BEGIN_EVENT_TABLE(chnlTimeLabel, wxWindow)
    EVT_PAINT(chnlTimeLabel::OnPaint)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTimeLabel::chnlTimeLabel(wxWindow* i_pParent)
:	wxWindow(i_pParent, wxID_ANY),
	m_FramesPerSecond(24),
	m_bDisplayFrames(false),
	m_bDisplayMinutes(false)
{	
	// Since we are drawing the background ourself, we can set the custom flag
	// to prevent flickering:
	this->SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	this->compute_size();	
}

//--------------------------------------------------------------------
// SetFramesPerSecond
//--------------------------------------------------------------------
void chnlTimeLabel::SetFramesPerSecond(int i_FPS)
{
	m_FramesPerSecond = i_FPS;
	this->Refresh();
}
int chnlTimeLabel::GetFramesPerSecond() const
{
	return m_FramesPerSecond;
}

//--------------------------------------------------------------------
// SetDisplayFrames - display numbers as seconds or frames
//--------------------------------------------------------------------
void chnlTimeLabel::SetDisplayFrames(bool i_bDisplayFrames)
{
	m_bDisplayFrames = i_bDisplayFrames;
	this->Refresh();
}
bool chnlTimeLabel::GetDisplayFrames() const
{
	return m_bDisplayFrames;
}

//--------------------------------------------------------------------
// SetDisplayMinutes - display numbers as seconds or minutes:seconds
//--------------------------------------------------------------------
void chnlTimeLabel::SetDisplayMinutes(bool i_bDisplayMinutes)
{
	m_bDisplayMinutes = i_bDisplayMinutes;
	this->Refresh();
}
bool chnlTimeLabel::GetDisplayMinutes() const
{
	return m_bDisplayMinutes;
}

//--------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//--------------------------------------------------------------------
void chnlTimeLabel::update_size()
{	
	this->compute_size();
	this->Refresh();	
}

//----------------------------------------------------------------------------
// Adjust size of control based on expanded and categories of clips
//----------------------------------------------------------------------------
void chnlTimeLabel::compute_size()
{
	wxSize size(this->get_full_width(), 32);
	this->SetMinSize(size);
	this->SetSize(size);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeLabel::OnPaint(wxPaintEvent &i_Event)
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

	// Draw time hashes
	wxPen line_pen( *wxBLACK, 1, wxPENSTYLE_SOLID);
	wxPen thick_pen( *wxBLACK, 3, wxPENSTYLE_SOLID);

	int num_ticks = m_MaxTime-m_MinTime;	// how many ticks to draw
	int tick_group = 10;			// how often to have a bolder, larger tick
	float spacing = m_TimeScale;	// how far apart should the ticks be?
	float tick_time = 1.0f;			// how much time does a tick represent?

	bool bShowFrameTicks = (m_TimeScale > (3 * m_FramesPerSecond));
	if (bShowFrameTicks)
	{
		num_ticks = (m_MaxTime-m_MinTime) * m_FramesPerSecond; 
		tick_group = m_FramesPerSecond;	
		spacing = (m_TimeScale / (float)m_FramesPerSecond);	
		tick_time = (1.0f / (float)m_FramesPerSecond);
	}

	int tick_shift = (m_MinTime < 0) ? ((int) (m_MinTime / tick_time - 0.5f)) :
									   ((int) (m_MinTime / tick_time + 0.5f));
	for (int i=0; i<=num_ticks; i++)
	{
		int pos = (int)(i * spacing + c_EdgeOffset);
		if ((i+tick_shift)%tick_group == 0)
		{
			dc.SetPen(thick_pen);
			dc.DrawLine(pos, 20, pos, 24);
		}
		else
		{
			dc.SetPen(line_pen);
			dc.DrawLine(pos, 21, pos, 24);
		}
	}

	// Always a fat tick at begin and end time
	int begin_pos = get_position_for_time(m_MinTime);
	int end_pos = get_position_for_time(m_MaxTime);
	dc.SetPen(thick_pen);
	dc.DrawLine(begin_pos, 20, begin_pos, 24);
	dc.DrawLine(end_pos, 20, end_pos, 24);

	// Label time numbers
	bool bDisplayMinutes = (m_bDisplayMinutes && !m_bDisplayFrames);
	wxFont text_font( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
	dc.SetFont(text_font);
	int skip = (m_TimeScale > 25) ? 1 : 10;
	int mod = (m_bDisplayFrames) ? m_FramesPerSecond : 1;
	float last_time_to_number = m_MaxTime-(tick_time*0.5f); // have to stop numbering close to end time
	float first_time_to_number = m_MinTime+(tick_time*0.5f);
	int start_second = (int)first_time_to_number + skip;
	if (first_time_to_number < 0) start_second -= 1; // (int) cast moves towards zero
	for (int i=start_second; i<=last_time_to_number; i+=skip)
	{
		//int pos = (int)(i * m_TimeScale + c_EdgeOffset);
		int pos = get_position_for_time((float)i);
		draw_num(dc, i*mod, pos, 0, bDisplayMinutes);
	}

	// Draw first and last numbers
	//draw_num(dc, 0, c_EdgeOffset, 0, bDisplayMinutes);
	//draw_num(dc, mod*(int)m_TotalTime, end_pos, 0, bDisplayMinutes);

	// Draw end time in complete format - it doesn't always align with an
	// even second in order to use an "int" in the code above.
	std::string begin_time_string;
	tmlnTimeUtil::GetTimeString(maTime::FromSeconds(m_MinTime), begin_time_string);
	wxString begin_str(begin_time_string.c_str(), wxConvUTF8);
	wxSize text_size = dc.GetTextExtent(begin_str);
	float center_off = text_size.GetWidth() * 0.5f;
	dc.DrawText(begin_str, begin_pos-center_off, 0); 

	std::string end_time_string;
	tmlnTimeUtil::GetTimeString(maTime::FromSeconds(m_MaxTime), end_time_string);
	wxString end_str(end_time_string.c_str(), wxConvUTF8);
	text_size = dc.GetTextExtent(end_str);
	center_off = text_size.GetWidth() * 0.5f;
	dc.DrawText(end_str, end_pos-center_off, 0); 
}

#endif // USE_WXWIDGETS
