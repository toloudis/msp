/****************************************************************************\
**	chnlTraxFrame.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "chnlTraxFrame.hpp"

#include "Features/Channels/wxGUI/chnlMarkerBar.hpp"
#include "Features/Channels/wxGUI/chnlTimeLabel.hpp"
#include "Features/Channels/wxGUI/chnlTimeSlider.hpp"

#include "ToolUIWx/twc/twcRangedFloat.hpp"

#include <algorithm>
#include <sstream>

namespace
{
	const float c_InitialZoom = 80.0f;

	//--------------------------------------------------------------------
	// Derived class in order to have the channel scrolling window
	//	also scroll the timeline and the channel labels
	//--------------------------------------------------------------------
	class ChannelScrollingWindow : public wxScrolledWindow
	{
	public:
		ChannelScrollingWindow(wxWindow* parent,
							   wxScrolledWindow* i_pTimeline,
							   wxScrolledWindow* i_pNames)
			: wxScrolledWindow(parent),
				m_pTimeline(i_pTimeline),
				m_pNames(i_pNames),
				m_bIgnoreNotify(false)
		{
		} 
		virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL )
		{
			wxScrolledWindow::ScrollWindow(dx, dy, rect);

			if (!m_bIgnoreNotify)
			{
				m_bIgnoreNotify = true;
				// Scroll the other two windows we control to match our scrolling
				int vbX,vbY;                     
				this->GetViewStart(&vbX,&vbY);
				m_pTimeline->Scroll(vbX, 0);
				m_pNames->Scroll(0, vbY);
				m_bIgnoreNotify = false;
			}
		}
		void NotifyScroll(int dx, int dy)
		{
			if (!m_bIgnoreNotify)
			{
				m_bIgnoreNotify = true;
				int vbX,vbY;                     
				this->GetViewStart(&vbX,&vbY);
				this->Scroll(vbX+(dx/5), vbY+(dy/5));
				m_bIgnoreNotify = false;
			}
		}
	private:
		wxScrolledWindow* m_pTimeline;
		wxScrolledWindow* m_pNames;
		bool m_bIgnoreNotify;
	};
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	class ServantScrollingWindow : public wxScrolledWindow
	{
	public:
		ServantScrollingWindow(wxWindow* parent, bool i_bHorizontal)
			: wxScrolledWindow(parent),
				m_pChannels(NULL),
				m_bHorizontal(i_bHorizontal)
		{	}
		void SetChannelWindow(ChannelScrollingWindow* i_pChannels)
		{
			m_pChannels = i_pChannels;
		} 
		virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL )
		{
			wxScrolledWindow::ScrollWindow(dx, dy, rect);
			if (m_pChannels)
			{
				if (m_bHorizontal)
					m_pChannels->NotifyScroll(-dx, 0);
				else
					m_pChannels->NotifyScroll(0, -dy);
			}
		}
	private:
		ChannelScrollingWindow* m_pChannels;
		bool m_bHorizontal;
	};


} // end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxFrame::chnlTraxFrame( wxWindow* parent, 
							  wxWindowID id, 
							  const wxString& title, 
							  const wxPoint& pos, 
							  const wxSize& size, 
							  long style ) 
: wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxFlexGridSizer* fgSizer_dialog;
	fgSizer_dialog = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer_dialog->AddGrowableCol( 1 );
	fgSizer_dialog->AddGrowableRow( 1 );
	fgSizer_dialog->SetFlexibleDirection( wxBOTH );
	fgSizer_dialog->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer_upperleft;
	fgSizer_upperleft = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer_upperleft->AddGrowableCol( 1 );
	fgSizer_upperleft->SetFlexibleDirection( wxBOTH );
	fgSizer_upperleft->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer_zoom;
	bSizer_zoom = new wxBoxSizer( wxHORIZONTAL );
	
	// .NET slider has range 0-500, value 80, num ticks 100, exponent 2
	// Removing the exponent gives range approx. 0-22, value 9
	// Since wxSlider is only integer, need to scale up by 10,
	//	so, make range 0-220, value 90; interpret the exponent on value changed
	//m_slider_zoom = new wxSlider( this, wxID_ANY, 90, 0, 220, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL );
	m_slider_zoom = new twcRangedFloat( this );
	m_slider_zoom->ShowValue( false );
	m_slider_zoom->SetMaximum( 500 );
	m_slider_zoom->SetExponent( 2 );
	m_slider_zoom->SetValue( c_InitialZoom );
	bSizer_zoom->Add( m_slider_zoom, 1, wxBOTTOM|wxTOP, 2 );
	this->Connect( m_slider_zoom->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(chnlTraxFrame::ZoomChanged) );
	
	m_button_zoomCenter = new wxButton( this, wxID_ANY, wxT("Z"), wxDefaultPosition, wxSize( 20,20 ), 0 );
	bSizer_zoom->Add( m_button_zoomCenter, 0, wxBOTTOM|wxTOP, 5 );
	
	fgSizer_upperleft->Add( bSizer_zoom, 1, wxEXPAND, 5 );
	
	wxArrayString m_choice_DisplayChoices;
	m_choice_Display = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 120,-1 ), m_choice_DisplayChoices, 0 );
	m_choice_Display->SetSelection( 0 );
	fgSizer_upperleft->Add( m_choice_Display, 0, wxALL, 5 );
	
	m_checkBox_Snap = new wxCheckBox( this, wxID_ANY, wxT("Snap"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer_upperleft->Add( m_checkBox_Snap, 0, wxALIGN_BOTTOM|wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer_Notes;
	bSizer_Notes = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer_Notes->Add( m_staticText1, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_RemoveNote = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer4->Add( m_button_RemoveNote, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_AddNote = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer4->Add( m_button_AddNote, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Notes->Add( bSizer4, 0, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_PrevNote = new wxButton( this, wxID_ANY, wxT("<"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer5->Add( m_button_PrevNote, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_NextNote = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer5->Add( m_button_NextNote, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Notes->Add( bSizer5, 0, wxALL|wxEXPAND, 2 );
	
	bSizer2->Add( bSizer_Notes, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer_Markers;
	bSizer_Markers = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText11 = new wxStaticText( this, wxID_ANY, wxT("Markers"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer_Markers->Add( m_staticText11, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_RemoveMarker = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer41->Add( m_button_RemoveMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_AddMarker = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer41->Add( m_button_AddMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Markers->Add( bSizer41, 0, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_PrevMarker = new wxButton( this, wxID_ANY, wxT("<"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer51->Add( m_button_PrevMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_NextMarker = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, wxSize( 16,16 ), 0 );
	bSizer51->Add( m_button_NextMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Markers->Add( bSizer51, 0, wxALL|wxEXPAND, 2 );
	
	bSizer2->Add( bSizer_Markers, 1, wxEXPAND, 5 );
	
	fgSizer_upperleft->Add( bSizer2, 1, wxEXPAND, 5 );
	
	fgSizer_dialog->Add( fgSizer_upperleft, 1, wxEXPAND, 5 );
	
	//m_scrollwin_timeline = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	//m_scrollwin_timeline = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	ServantScrollingWindow *servant_timeline = new ServantScrollingWindow( this, true );
	m_scrollwin_timeline = servant_timeline;
	m_scrollwin_timeline->SetScrollRate( 5, 0 );
	m_bSizer_timeline = new wxBoxSizer( wxVERTICAL );
	
	m_timeLabel = new chnlTimeLabel( m_scrollwin_timeline );
	m_bSizer_timeline->Add( m_timeLabel, 0, wxALL, 0 );
	//m_timeLabel->SetTimeScale( c_InitialZoom );
	
	m_timeSlider = new chnlTimeSlider( m_scrollwin_timeline );
	m_timeSlider->SetBackgroundColour(*wxWHITE);
	this->Connect( m_timeSlider->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(chnlTraxFrame::TimeChanged) );
	m_bSizer_timeline->Add( m_timeSlider, 0, wxALL, 0 );
	//m_timeSlider->SetTimeScale( c_InitialZoom );
	
	m_markerBar = new chnlMarkerBar( m_scrollwin_timeline );
	m_markerBar->SetBackgroundColour(*wxWHITE);
	m_bSizer_timeline->Add( m_markerBar, 0, wxALL, 0 );
 
	m_bSizer_timeline->AddStretchSpacer( 1 ); //spacer
	
	m_scrollwin_timeline->SetSizer( m_bSizer_timeline );
	m_scrollwin_timeline->Layout();
	m_bSizer_timeline->Fit( m_scrollwin_timeline );
	fgSizer_dialog->Add( m_scrollwin_timeline, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer_left;
	bSizer_left = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizer_buttons;
	bSizer_buttons = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button17 = new wxButton( this, wxID_ANY, wxT("<-"), wxDefaultPosition, wxSize( 25,25 ), 0 );
	bSizer16->Add( m_button17, 0, wxLEFT, 5 );
	
	m_button18 = new wxButton( this, wxID_ANY, wxT("->"), wxDefaultPosition, wxSize( 25,25 ), 0 );
	bSizer16->Add( m_button18, 0, wxRIGHT, 5 );
	
	bSizer_buttons->Add( bSizer16, 0, wxEXPAND, 5 );
	
	m_button_Add = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Add, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Delete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Delete, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Copy = new wxButton( this, wxID_ANY, wxT("Copy"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Copy, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Paste = new wxButton( this, wxID_ANY, wxT("Paste"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Paste, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Split = new wxButton( this, wxID_ANY, wxT("Split"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Split, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Move = new wxButton( this, wxID_ANY, wxT("Move"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Move, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Sel = new wxButton( this, wxID_ANY, wxT("Sel->"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer_buttons->Add( m_button_Sel, 0, wxLEFT|wxRIGHT, 5 );
	
	bSizer_left->Add( bSizer_buttons, 0, wxEXPAND, 5 );
	
	ServantScrollingWindow *servant_names = new ServantScrollingWindow( this, false );
	m_scrollwin_names = servant_names;
	//m_scrollwin_names = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	//m_scrollwin_names = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_scrollwin_names->SetScrollRate( 0, 5 );

	// Sizer for the channel names
	m_bSizer_names = new wxBoxSizer( wxVERTICAL );
	
	//m_staticText_category = new wxStaticText( m_scrollwin_names, wxID_ANY, wxT("Category"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_staticText_category->Wrap( -1 );
	//m_staticText_category->SetMinSize(wxSize(-1, 20));
	//m_staticText_category->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	//m_staticText_category->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	//m_bSizer_names->Add( m_staticText_category, 0, wxALL|wxEXPAND, 5 );
	
	//wxBoxSizer* bSizer18;
	//bSizer18 = new wxBoxSizer( wxHORIZONTAL );
	//
	//
	//bSizer18->Add( 10, 0, 0, wxEXPAND, 5 );
	//
	//m_staticText_channelName = new wxStaticText( m_scrollwin_names, wxID_ANY, wxT("Channel Name"), wxDefaultPosition, wxDefaultSize, 0 );
	//m_staticText_channelName->Wrap( -1 );
	//bSizer18->Add( m_staticText_channelName, 1, wxALL, 5 );
	//
	//m_checkBox_locked = new wxCheckBox( m_scrollwin_names, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	//
	//bSizer18->Add( m_checkBox_locked, 0, wxALL, 5 );
	//
	//m_bSizer_names->Add( bSizer18, 1, wxEXPAND, 5 );
	
	m_scrollwin_names->SetSizer( m_bSizer_names );
	m_scrollwin_names->Layout();
	m_bSizer_names->Fit( m_scrollwin_names );
	bSizer_left->Add( m_scrollwin_names, 1, wxEXPAND | wxALL, 5 );
	
	fgSizer_dialog->Add( bSizer_left, 1, wxEXPAND, 5 );
	
	//m_scrollwin_channels = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxVSCROLL );
	ChannelScrollingWindow *channel_window = new ChannelScrollingWindow( this, m_scrollwin_timeline, m_scrollwin_names );
	m_scrollwin_channels = channel_window;
	m_scrollwin_channels->SetScrollRate( 5, 5 );
	m_scrollwin_channels->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	//this->Connect( m_scrollwin_channels->GetId(), wxEVT_SCROLLWIN_THUMBTRACK,
	//				wxScrollWinEventHandler(chnlTraxFrame::ScrollChanged) );
	//m_scrollwin_channels->SetTargetWindow( m_scrollwin_timeline );

	// Allow timeline and name scrollbars to alter channel scrollbar also
	servant_timeline->SetChannelWindow(channel_window);
	servant_names->SetChannelWindow(channel_window);

	m_bSizer_channels = new wxBoxSizer( wxVERTICAL );
	
	m_scrollwin_channels->SetSizer( m_bSizer_channels );
	m_scrollwin_channels->Layout();
	m_bSizer_channels->Fit( m_scrollwin_channels );
	fgSizer_dialog->Add( m_scrollwin_channels, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( fgSizer_dialog );
	this->Layout();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxFrame::~chnlTraxFrame()
{
}

//--------------------------------------------------------------------
// Remove all channel  controls from form
//--------------------------------------------------------------------
void chnlTraxFrame::ClearChannels()
{
	m_Channels.clear();

	const bool bDeleteAll = true;
	m_bSizer_names->Clear(bDeleteAll);
	m_bSizer_channels->Clear(bDeleteAll);
}

//--------------------------------------------------------------------
// Add category label to form
//--------------------------------------------------------------------
void chnlTraxFrame::AddCategory(const std::string& i_CategoryName)
{
	wxStaticText *pCategory = new wxStaticText( m_scrollwin_names, wxID_ANY, i_CategoryName, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	pCategory->Wrap( -1 );
	pCategory->SetMinSize(wxSize(-1, 20));
	pCategory->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	pCategory->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	m_bSizer_names->Add( pCategory, 0, wxALL|wxEXPAND, 5 );

	// Add spacer on channel side of controls
	m_bSizer_channels->Add(0, 20, 0, wxALL|wxEXPAND, 5);
}

//--------------------------------------------------------------------
// Add channel control and label to form
//--------------------------------------------------------------------
chnlChannelControl* chnlTraxFrame::AddChannel(const std::string& i_ChannelName)
{	
	// Box sizer holds spacer, channel name label, check box for locked
	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );
	//wxStaticBoxSizer* bSizer = new wxStaticBoxSizer( wxHORIZONTAL, m_scrollwin_names );
	bSizer->SetMinSize( wxSize( -1, 32 ) ); 
	bSizer->Add( 10, 0, 0, wxEXPAND, 5 ); //spacer
	// Channel name label
	wxStaticText *pLabel = new wxStaticText( m_scrollwin_names, wxID_ANY, i_ChannelName, wxDefaultPosition, wxDefaultSize, 0 );
	pLabel->Wrap( -1 );
	bSizer->Add( pLabel, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	// Check box for locked state of channel
	wxCheckBox* pLocked = new wxCheckBox( m_scrollwin_names, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( pLocked, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	// Add to sizer in scrolled window on left
	m_bSizer_names->Add( bSizer, 0, wxEXPAND, 5 );
	m_scrollwin_names->Layout();
	
	// Add channel control to main scrolled window in lower right
	chnlChannelControl *pChannelCtrl = new chnlChannelControl( m_scrollwin_channels );
	m_bSizer_channels->Add( pChannelCtrl, 0, wxLEFT|wxRIGHT, 0 );
	m_scrollwin_channels->Layout();

	pChannelCtrl->SetInteractionCallback(this);
	m_Channels.push_back(pChannelCtrl);

	return pChannelCtrl;
}

//--------------------------------------------------------------------
// SetTotalTime changes length of timeline controls
//--------------------------------------------------------------------
void chnlTraxFrame::SetTotalTime(float i_Time)
{
	if (i_Time > 0)
	{
		this->m_timeLabel->SetTotalTime( i_Time );
		this->m_timeSlider->SetTotalTime( i_Time );
		this->m_markerBar->SetTotalTime( i_Time );

		// Set time scale for channels also
		std::for_each(m_Channels.begin(), m_Channels.end(),
			std::bind2nd(std::mem_fun(&chnlChannelControl::SetTotalTime),
						 i_Time));

		resize_scrolled_windows();
	}
}


//--------------------------------------------------------------------
// TimeScale changes the zoom left/right
//--------------------------------------------------------------------
void chnlTraxFrame::SetTimeScale(float i_Scale)
{
	if (i_Scale > 0)
	{
		this->m_timeLabel->SetTimeScale( i_Scale );
		this->m_timeSlider->SetTimeScale( i_Scale );
		this->m_markerBar->SetTimeScale( i_Scale );

		// Set time scale for channels also
		std::for_each(m_Channels.begin(), m_Channels.end(),
			std::bind2nd(std::mem_fun(&chnlChannelControl::SetTimeScale),
						 i_Scale));

		////	normally this would be called ONLY when the scale changes
		//TimeGuideLineMgr::UpdateScale( i_Scale );

		////	adjust the timeline position based on the selected object or current time.
		////
		//center_view();

		resize_scrolled_windows();
	}
}

//--------------------------------------------------------------------
// AddTimeTick adds tick to time slider
//--------------------------------------------------------------------
void chnlTraxFrame::AddTimeTick(float i_Time)
{
	this->m_timeSlider->AddTimeTick(i_Time);
}

//--------------------------------------------------------------------
// AddMarker adds marker to marker bar
//--------------------------------------------------------------------
void chnlTraxFrame::AddMarker(float i_Time, 
							chnlMarkerIcon::MarkerType i_Type, 
							const std::string& i_Note)
{
	this->m_markerBar->AddMarker(i_Time, i_Type, i_Note);
}

//--------------------------------------------------------------------
// AddNote adds note to marker bar
//--------------------------------------------------------------------
void chnlTraxFrame::AddNote(float i_Time, 
							chnlNoteIcon::NoteStatus i_Status, 
							const std::string& i_Note)
{
	this->m_markerBar->AddNote(i_Time, i_Status, i_Note);
}

//--------------------------------------------------------------------
// Interaction Callbacks from channel controls
//--------------------------------------------------------------------
//virtual 
void chnlTraxFrame::ClipSelected(chnlChannelControl* i_pChannel, chnlChannelClip* i_pClip, bool i_bAppended)
{
	// If we are clearing the selection, or not appending to 
	// the selection, then clear selection from other channels
	if (i_pClip == NULL || i_bAppended == false)
	{
		const int num_channels = m_Channels.size();
		for (int i=0; i<num_channels; i++)
		{	
			if (m_Channels[i] != i_pChannel)
				m_Channels[i]->ClearSelection();
		}
	}

	// Prepare all selected clips in all channels for the interaction
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fun(&chnlChannelControl::BeginInteraction));
}
//virtual 
void chnlTraxFrame::ClipMoved(chnlChannelControl*, float i_TimeDelta)
{
	// Slide all selected clips in all channels for the interaction
	std::for_each(m_Channels.begin(), m_Channels.end(), 
		std::bind2nd(std::mem_fun(&chnlChannelControl::InteractMoveSelectedClips),
					 i_TimeDelta));

}
//virtual 
void chnlTraxFrame::ClipMoveFinished(chnlChannelControl*)
{
	// End interaction in all channels
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fun(&chnlChannelControl::FinishInteraction));
}
//virtual 
void chnlTraxFrame::ClipResized(chnlChannelControl*)
{

}

//----------------------------------------------------------------------------
// This will call ::Layout() and ::AdjustScrollbars()
// for the scrolled windows:
//----------------------------------------------------------------------------
void chnlTraxFrame::resize_scrolled_windows()
{
    wxSize size = m_scrollwin_timeline->GetBestVirtualSize();
    m_scrollwin_timeline->SetVirtualSize( size );
    size = m_scrollwin_channels->GetBestVirtualSize();
    m_scrollwin_channels->SetVirtualSize( size );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxFrame::TimeChanged(wxCommandEvent &i_Event)
{
	std::ostringstream str;
	str << m_timeSlider->GetCurrentTime();
	this->SetTitle( str.str() );
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxFrame::ZoomChanged(wxCommandEvent &i_Event)
{
	this->SetTimeScale( m_slider_zoom->GetValue() );
}

