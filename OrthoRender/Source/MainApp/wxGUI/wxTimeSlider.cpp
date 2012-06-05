/*****************************************************************************
**  wxTimeSlider.hpp
**
**     Time slider in main form when using wxWidgets
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/wxGUI/wxTimeSlider.hpp"

#include "Support/ptm/wxGUI/ptmTimeEdit.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"

#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------
wxTimeSlider::wxTimeSlider( wxWindow* parent, 
						    wxWindowID id, 
							const wxPoint& pos, 
							const wxSize& size, 
							long style ) 
: wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	// Min Time
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_floatEdit_MinTime = new ptmTimeEdit( this, wxID_ANY, wxDefaultPosition, wxSize(80,-1), wxTE_CENTRE );
	m_floatEdit_MinTime->SetTimeValue(tmlnTimeLine::GetMinimum());
	this->Connect( m_floatEdit_MinTime->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(wxTimeSlider::MinTimeChanged) );
	bSizer4->Add( m_floatEdit_MinTime, 0, wxALL, 0 );
	
	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Min time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer4->Add( m_staticText3, 0, wxALIGN_CENTER|wxALL, 0 );
	
	bSizer1->Add( bSizer4, 0, wxALL, 5 );

	// Slider
	m_slider1 = new twcRangedFloat( this );
	m_slider1->ShowValue( false );
	m_slider1->SetMaximum( tmlnTimeLine::GetMaximum() );
	m_slider1->SetNumTicks( tmlnTimeLine::GetMaximum() * tmlnTimeLine::GetFPS());
	this->Connect( m_slider1->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(wxTimeSlider::SliderChanged) );	
	this->Connect( wxID_ANY, wxEVT_SCROLL_THUMBTRACK,
				wxCommandEventHandler(wxTimeSlider::SliderThumbDown) );
	this->Connect( wxID_ANY, wxEVT_SCROLL_THUMBRELEASE,
				wxCommandEventHandler(wxTimeSlider::SliderThumbUp) );
	bSizer1->Add( m_slider1, 1, wxALL, 2 );
	
	// Current Time
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_floatEdit_CurrentTime = new ptmTimeEdit( this, wxID_ANY, wxDefaultPosition, wxSize(80,-1), wxTE_CENTRE );

	m_floatEdit_CurrentTime->SetTimeValue(tmlnTimeLine::GetValue());
	this->Connect( m_floatEdit_CurrentTime->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(wxTimeSlider::CurrentTimeChanged) );
	bSizer2->Add( m_floatEdit_CurrentTime, 0, wxALL, 0 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Current time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer2->Add( m_staticText1, 0, wxALIGN_CENTER|wxALL, 0 );
	
	bSizer1->Add( bSizer2, 0, wxALL, 5 );
	
	// Max Time
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_floatEdit_MaxTime = new ptmTimeEdit( this, wxID_ANY, wxDefaultPosition, wxSize(80,-1), wxTE_CENTRE );
	m_floatEdit_MaxTime->SetTimeValue(tmlnTimeLine::GetMaximum());
	this->Connect( m_floatEdit_MaxTime->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(wxTimeSlider::MaxTimeChanged) );
	bSizer3->Add( m_floatEdit_MaxTime, 0, wxALL, 0 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Max time"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 0 );
	
	bSizer1->Add( bSizer3, 0, wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();

	// Add this panel to the AUI manager
	const char* c_Title = "Time Slider";
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(c_Title).Caption(c_Title).Show().Layer(2).Bottom());

	// Add this control as a time interest
	tmlnTimeLine::AddTimeInterest(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
wxTimeSlider::~wxTimeSlider()
{
	// Unregister time interest
	tmlnTimeLine::RemoveTimeInterest(this);
}

//--------------------------------------------------------------------
//	TimeChanged - timeline current time has changed
//--------------------------------------------------------------------
//virtual 
void wxTimeSlider::TimeChanged( float i_Time )
{
	m_floatEdit_CurrentTime->SetTimeValue(tmlnTimeLine::GetValue());
	m_slider1->SetValue( tmlnTimeLine::GetValue() );
}

//--------------------------------------------------------------------
//	TimeRangeChanged - timeline maximum time has changed
//--------------------------------------------------------------------
//virtual 
void wxTimeSlider::TimeRangeChanged( float i_MinTime, float i_MaxTime )
{
	m_floatEdit_MinTime->SetTimeValue(i_MinTime);
	m_floatEdit_MaxTime->SetTimeValue(i_MaxTime);
	
	m_slider1->SetRange( i_MinTime, i_MaxTime );

	float num_frames = (i_MaxTime - i_MinTime) * tmlnTimeLine::GetFPS();
	if (num_frames < 0) num_frames = 0;
	m_slider1->SetNumTicks( num_frames );
}

//--------------------------------------------------------------------
//	TimeFormatChanged - timeline time display format has changed
//--------------------------------------------------------------------
//virtual 
void wxTimeSlider::TimeFormatChanged( int i_TimeFormat )
{
}

//----------------------------------------------------------------------------
// Turn scrubbing on when mouse is down on timeline
//----------------------------------------------------------------------------
void wxTimeSlider::SliderThumbDown(wxCommandEvent &i_Event)
{
	tmlnTimeLine::SetIsScrubbing( true );
	i_Event.Skip();
}
void wxTimeSlider::SliderThumbUp(wxCommandEvent &i_Event)
{
	tmlnTimeLine::SetIsScrubbing( false );
	i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxTimeSlider::SliderChanged(wxCommandEvent &i_Event)
{
	float cur_time = m_slider1->GetValue();
	tmlnTimeUtil::AdjustTimeToFrame(cur_time);
	tmlnTimeLine::SetValue(cur_time);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxTimeSlider::CurrentTimeChanged(wxCommandEvent &i_Event)
{
	float cur_time = m_floatEdit_CurrentTime->GetTimeValue();
	tmlnTimeUtil::AdjustTimeToFrame(cur_time);
	tmlnTimeLine::SetValue(cur_time);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxTimeSlider::MaxTimeChanged(wxCommandEvent &i_Event)
{
	float max_time = m_floatEdit_MaxTime->GetTimeValue();
	tmlnTimeUtil::AdjustTimeToFrame(max_time);
	//tmlnTimeLine::SetMaximum(max_time);
	tmlnTimeLine::SetTimeRange(tmlnTimeLine::GetMinimum(), max_time);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void wxTimeSlider::MinTimeChanged(wxCommandEvent &i_Event)
{
	float min_time = m_floatEdit_MinTime->GetTimeValue();
	tmlnTimeUtil::AdjustTimeToFrame(min_time);
	tmlnTimeLine::SetTimeRange(min_time, tmlnTimeLine::GetMaximum());
}


#endif	// USE_WXWIDGETS
