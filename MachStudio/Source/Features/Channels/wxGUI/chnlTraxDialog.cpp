/****************************************************************************\
**	chnlTraxDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlTraxDialog.hpp"

#include "Features/Channels/chnlOperations.hpp"
#include "Features/Channels/chnlSnapUtil.hpp"
#include "Features/Channels/chnlTimeTickMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerMgr.hpp"
#include "Features/Channels/Markers/chnlMarkerOperations.hpp"
#include "Features/Channels/Notes/chnlNotesMgr.hpp"
#include "Features/Channels/Notes/chnlNotesOperations.hpp"
#include "Features/Channels/wxGUI/chnlClipDriver.hpp"
#include "Features/Channels/wxGUI/chnlMarkerBar.hpp"
#include "Features/Channels/wxGUI/chnlTimeLabel.hpp"
#include "Features/Channels/wxGUI/chnlTimelineGuideUtil.hpp"
//#include "Features/Channels/wxGUI/chnlTimelineMarquee.hpp"
#include "Features/Channels/wxGUI/chnlTimelinePanel.hpp"
#include "Features/Channels/wxGUI/chnlTimeSlider.hpp"
#include "Features/Prefs/PrefsMgr.hpp"

#undef GetTimeFormat

#include "Support/tmln/tmlnChannel.hpp"
#include "Support/tmln/tmlnChannelSet.hpp"
#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnTimeEditUIInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/Ma/maFunctions.hpp"
#include "Core/Fs/fsFileUtil.hpp"
//#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiPropertyDialog.hpp"
#include "ToolUIWx/twc/twcRangedFloat.hpp"
#include "ToolUIWx/twx/twxMessaging.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	wxString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return wxString(wide_dir.GetString());
	}
   
	const float c_InitialZoom = 60.0f;
	const int c_DefaultSnapInterval = 120;

	//--------------------------------------------------------------------
	// Derived class in order to have the channel scrolling window
	//	also scroll the timeline and the channel labels
	//--------------------------------------------------------------------
	class ChannelScrollingWindow : public chnlTimelinePanel
	{
	public:
		ChannelScrollingWindow(wxWindow* parent,
							   wxScrolledWindow* i_pTimeline,
							   wxScrolledWindow* i_pNames)
			: chnlTimelinePanel(parent),
				m_pTimeline(i_pTimeline),
				m_pNames(i_pNames),
				m_bIgnoreNotify(false)
		{
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
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


	//--------------------------------------------------------------------
	// Check box for locking a channel
	//--------------------------------------------------------------------
	class ChannelLockedBitmapButton : public wxBitmapButton
	{
	public:
		ChannelLockedBitmapButton(wxWindow* parent,tmlnChannel &i_Channel, 
								  wxBitmap unlock, wxBitmap lock, wxString& name)
		:	wxBitmapButton(parent, wxID_ANY, unlock, wxDefaultPosition, wxDefaultSize,
			wxBU_AUTODRAW, wxDefaultValidator,name), m_Channel(i_Channel), 
			m_pChannelCtrl(NULL)
			  
		{
			wxString icon_dir = get_icon_directory();
		
			if(i_Channel.IsLocked())
				this->SetBitmapLabel(lock);
			else
				this->SetBitmapLabel(unlock);
			
			// connect callback to checkbox
			this->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChannelLockedBitmapButton::bitmapButton_OnClick ), NULL, this );
		}
		
		void AssignChannelControl(chnlChannelControl *i_pChannelCtrl)
		{
			m_pChannelCtrl = i_pChannelCtrl;
		}
		
		void SetBitmaps(wxBitmap i_unlock, wxBitmap i_lock)
		{
			Lock = i_lock;
			Unlock = i_unlock;
		}
	private:
		tmlnChannel &m_Channel;
		chnlChannelControl *m_pChannelCtrl;
		wxBitmap Lock, Unlock;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void bitmapButton_OnClick( wxCommandEvent& i_Event )
		{ 
			bool bChecked = m_Channel.IsLocked()?false:true;
			wxString icon_dir = get_icon_directory();
			m_Channel.ChannelLocked(bChecked);
			if (m_pChannelCtrl) m_pChannelCtrl->SetLocked(bChecked);
			if (bChecked)
				this->SetBitmapLabel(Lock);
			else 
				this->SetBitmapLabel(Unlock);
			
		}
	};

	//--------------------------------------------------------------------
	// Force the scrolled window to adjust its scrollbars
	//--------------------------------------------------------------------
	void adjust_scrollbars(wxScrolledWindow* i_pScrolledWin)
	{
		wxSize size = i_pScrolledWin->GetBestVirtualSize();
		// This will call ::Layout() and ::AdjustScrollbars()
		i_pScrolledWin->SetVirtualSize( size );
	}

	//--------------------------------------------------------------------
	// convert clip to a driver clip type and 
	// get the driver pointer from it
	//--------------------------------------------------------------------
	tmlnDriver* get_driver_for_clip(chnlChannelClip* i_pClip)
	{
		// Cast clip to our driver clip type in order
		// to get the tmlnDriver
		chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (clip)
		{
			tmlnDriver* driver = &(clip->Driver());
			return driver;
		}
		return NULL;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	enum AlignType
	{
		e_NoMatch = -1,
		e_TimeTick = 0,
		e_Marker,
		e_CurrentTime
	};

	//--------------------------------------------------------------------
	// Align given time with important other times like markers and
	//	time ticks and the current time.
	//--------------------------------------------------------------------
	bool align_time(const maTime& i_Time, maTime &o_ClosestTime, AlignType& o_AlignType)
	{
		bool bMatch = false;

		maTime mtime;
		if (chnlTimeTickMgr::SnapTimeToTicks(i_Time, mtime))
		{
			bMatch = true;
			o_ClosestTime = mtime;
			o_AlignType = e_TimeTick;
		}
		if (chnlMarkerMgr::SnapTimeToMarkers(i_Time, mtime))
		{
			bMatch = true;
			o_ClosestTime = mtime;
			o_AlignType = e_Marker;
		}

		maTime curtime = tmlnTimeLine::GetValue();
		if (chnlSnapUtil::TimesMatch(i_Time, curtime))
		{
			bMatch = true;
			o_ClosestTime = curtime;
			o_AlignType = e_CurrentTime;
		}

		return bMatch;
	}

	//--------------------------------------------------------------------
	// Align given clip with important times
	//--------------------------------------------------------------------
	void align_moving_clip(chnlChannelClip* i_pClip)
	{
		AlignType align_type = e_NoMatch;
		maTime match_time;
		if (align_time(i_pClip->GetInteractStart(), match_time, align_type))
		{
			i_pClip->AlignInteractStart( match_time );
		}
		else if (align_time(i_pClip->GetInteractEnd(), match_time, align_type))
		{
			i_pClip->AlignInteractEnd( match_time );
		}
		
		switch (align_type)
		{
			default:
			case e_NoMatch:
				chnlTimelineGuideUtil::HideMarkerGuide();
				break;
			case e_TimeTick:
				chnlTimelineGuideUtil::ShowMarkerGuide(match_time, wxColour(0,200,0));
				break;
			case e_Marker:
			case e_CurrentTime:
				chnlTimelineGuideUtil::ShowMarkerGuide(match_time, wxColour(0,100,0));
				break;
		}
	}

	//--------------------------------------------------------------------
	// Align all selected clips in this control with important times
	//--------------------------------------------------------------------
	void align_selected_clips(chnlChannelControl* i_pChannelControl)
	{
		
		std::for_each(i_pChannelControl->GetSelectedClips().begin(),
					i_pChannelControl->GetSelectedClips().end(),
					align_moving_clip);
	}

} // end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
chnlTraxDialog* chnlTraxDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxDialog::chnlTraxDialog( wxWindow* parent, 
							  wxWindowID id, 
							  const wxString& title, 
							  const wxPoint& pos, 
							  const wxSize& size, 
							  long style ) 
//: wxDialog(parent, id, title, pos, size, style)
: wxPanel( parent, id, pos, size, style ),
	m_bDriverSelectStart(false),
	m_nFilterState(0),
	m_SnapInterval(c_DefaultSnapInterval),
	m_InitX(0),
	m_InitY(0),
	m_bleftIsDown(false),
	endPoint(0,0),
	savePoint(0,0),
	width(0),
	height(0),
	windowW(0),
	windowH(0)

{
	//	Initialize the GUI elements
	//
	wxInitAllImageHandlers();

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxFlexGridSizer* fgSizer_dialog;
	fgSizer_dialog = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer_dialog->AddGrowableCol( 1 );
	fgSizer_dialog->AddGrowableRow( 1 );
	fgSizer_dialog->SetFlexibleDirection( wxBOTH );
	fgSizer_dialog->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizer_upperleft;
	fgSizer_upperleft = new wxFlexGridSizer( 3, 3, 0, 0 );
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
	m_slider_zoom->SetValue( c_InitialZoom );
	m_slider_zoom->SetMinimum( 3 );
	m_slider_zoom->SetExponent( 2 );
	m_slider_zoom->SetToolTip( wxT("Zoom slider") );
	bSizer_zoom->Add( m_slider_zoom, 1, wxBOTTOM|wxTOP, 2 );
	this->Connect( m_slider_zoom->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(chnlTraxDialog::ZoomChanged) );
	
	m_button_ZoomCenter = new wxButton( this, wxID_ANY, wxT("Z"), wxDefaultPosition, FromDIP(wxSize( 20,20 )), 0 );
	m_button_ZoomCenter->SetToolTip( wxT("Fit time to Channel Editor size") );
	bSizer_zoom->Add( m_button_ZoomCenter, 0, wxBOTTOM|wxTOP, 5 );
	
	fgSizer_upperleft->Add( bSizer_zoom, 1, wxEXPAND, 5 );
	
	wxArrayString m_choice_DisplayChoices;
	m_choice_DisplayChoices.Add(wxT("Multiple Selected"));
	m_choice_DisplayChoices.Add(wxT("Selected"));
	m_choice_DisplayChoices.Add(wxT("Similar"));
	m_choice_DisplayChoices.Add(wxT("All"));
	m_choice_Display = new wxChoice( this, wxID_ANY, wxDefaultPosition, FromDIP(wxSize( 120,-1 )), m_choice_DisplayChoices, 0 );
	m_choice_Display->SetSelection( 0 );
	m_choice_Display->SetToolTip( wxT("Display Filter") );
	this->Connect(m_choice_Display->GetId(), wxEVT_COMMAND_CHOICE_SELECTED,
				wxCommandEventHandler(chnlTraxDialog::FilterIndexChanged) );
	fgSizer_upperleft->Add( m_choice_Display, 0, wxALL, 5 );
	
	fgSizer_upperleft->Add( 0, 0, 1, wxEXPAND, 5 );
	m_checkBox_Snap = new wxCheckBox( this, wxID_ANY, wxT("Snap"), wxDefaultPosition, wxDefaultSize, 0 );
	// These next two lines set snapping "on" by default
	this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
	m_checkBox_Snap->SetValue(true);
	this->Connect( m_checkBox_Snap->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
					wxCommandEventHandler(chnlTraxDialog::checkBox_Snap_Clicked) );

	fgSizer_upperleft->Add( m_checkBox_Snap, 0, wxALIGN_RIGHT, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer_Notes;
	bSizer_Notes = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Notes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer_Notes->Add( m_staticText1, 1, wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_RemoveNote = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_RemoveNote->SetToolTip( wxT("Delete Note") );
	bSizer4->Add( m_button_RemoveNote, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_AddNote = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_AddNote->SetToolTip( wxT("Add Note at current time") );
	bSizer4->Add( m_button_AddNote, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Notes->Add( bSizer4, 0, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_PrevNote = new wxButton( this, wxID_ANY, wxT("<"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_PrevNote->SetToolTip( wxT("Previous Note") );
	bSizer5->Add( m_button_PrevNote, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_NextNote = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_NextNote->SetToolTip( wxT("Next Note") );
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
	
	m_button_RemoveMarker = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_RemoveMarker->SetToolTip( wxT("Delete Marker") );
	bSizer41->Add( m_button_RemoveMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_AddMarker = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_AddMarker->SetToolTip( wxT("Add Marker") );
	bSizer41->Add( m_button_AddMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Markers->Add( bSizer41, 0, wxALL|wxEXPAND, 2 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button_PrevMarker = new wxButton( this, wxID_ANY, wxT("<"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_PrevMarker->SetToolTip( wxT("Previous Marker") );
	bSizer51->Add( m_button_PrevMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	m_button_NextMarker = new wxButton( this, wxID_ANY, wxT(">"), wxDefaultPosition, FromDIP(wxSize( 16,16 )), 0 );
	m_button_NextMarker->SetToolTip( wxT("Next Marker") );
	bSizer51->Add( m_button_NextMarker, 0, wxBOTTOM|wxTOP, 2 );
	
	bSizer_Markers->Add( bSizer51, 0, wxALL|wxEXPAND, 2 );
	
	bSizer2->Add( bSizer_Markers, 1, wxEXPAND, 5 );
	
	fgSizer_upperleft->Add( bSizer2, 1, wxEXPAND, 5 );
	fgSizer_upperleft->Add( 0, 0, 1, wxEXPAND, 5 );
	fgSizer_upperleft->AddGrowableCol( 0 );
	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0);
	fgSizer_upperleft->Add(m_staticText2, 0, wxALIGN_LEFT|wxALL, 5);
	fgSizer_upperleft->Add( 0, 0, 1, wxEXPAND, 5 );

	
	fgSizer_dialog->Add( fgSizer_upperleft, 1, wxEXPAND, 5 );
	
	//m_scrollwin_timeline = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	//m_scrollwin_timeline = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	ServantScrollingWindow *servant_timeline = new ServantScrollingWindow( this, true );
	m_scrollwin_timeline = servant_timeline;
	m_scrollwin_timeline->SetScrollRate( 5, 0 );
	m_bSizer_timeline = new wxBoxSizer( wxVERTICAL );
	
	m_timeLabel = new chnlTimeLabel( m_scrollwin_timeline );
	m_bSizer_timeline->Add( m_timeLabel, 0, wxALL, 0 );
	m_timeLabel->SetTimeScale( c_InitialZoom );
	
	m_timeSlider = new chnlTimeSlider( m_scrollwin_timeline );
	m_timeSlider->SetBackgroundColour(*wxWHITE);
	this->Connect( m_timeSlider->GetId(), wxEVT_VALUE_CHANGED,
				wxCommandEventHandler(chnlTraxDialog::TimeChanged) );
	m_bSizer_timeline->Add( m_timeSlider, 0, wxALL, 0 );
	m_timeSlider->SetTimeScale( c_InitialZoom );
	
	m_markerBar = new chnlMarkerBar(m_scrollwin_timeline);
	m_markerBar->SetBackgroundColour(*wxWHITE);
	m_bSizer_timeline->Add( m_markerBar, 0, wxALL, 0 );
	m_markerBar->SetTimeScale( c_InitialZoom );
 
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
	
	m_button_PrevTick = new wxButton( this, wxID_ANY, wxT("<-"), wxDefaultPosition, FromDIP(wxSize( 25,25 )), 0 );
	m_button_PrevTick->SetToolTip( wxT("Move time to the previous tick") );
	bSizer16->Add( m_button_PrevTick, 0, wxLEFT, 5 );
	
	m_button_NextTick = new wxButton( this, wxID_ANY, wxT("->"), wxDefaultPosition, FromDIP(wxSize( 25,25 )), 0 );
	m_button_NextTick->SetToolTip( wxT("Move time to the next tick") );
	bSizer16->Add( m_button_NextTick, 0, wxRIGHT, 5 );
	
	bSizer_buttons->Add( bSizer16, 0, wxEXPAND, 5 );
	
	m_button_Delete = new wxButton( this, wxID_ANY, wxT("Delete"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Delete->SetToolTip( wxT("Delete selected driver") );
	bSizer_buttons->Add( m_button_Delete, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Copy = new wxButton( this, wxID_ANY, wxT("Copy"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Copy->SetToolTip( wxT("Copy selected driver") );
	bSizer_buttons->Add( m_button_Copy, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Paste = new wxButton( this, wxID_ANY, wxT("Paste"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Paste->SetToolTip( wxT("Paste copied driver at current time") );
	bSizer_buttons->Add( m_button_Paste, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Split = new wxButton( this, wxID_ANY, wxT("Split"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Split->SetToolTip( wxT("Split selected driver at current time") );
	bSizer_buttons->Add( m_button_Split, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Move = new wxButton( this, wxID_ANY, wxT("Move"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Move->SetToolTip( wxT("Move driver") );
	bSizer_buttons->Add( m_button_Move, 0, wxLEFT|wxRIGHT, 5 );
	
	m_button_Sel = new wxButton( this, wxID_ANY, wxT("Sel->"), wxDefaultPosition, FromDIP(wxSize( 50,-1 )), 0 );
	m_button_Sel->SetToolTip( wxT("Select all drivers later in time of currently selected driver") );
	bSizer_buttons->Add( m_button_Sel, 0, wxLEFT|wxRIGHT, 5 );

	bSizer_left->Add( bSizer_buttons, 0, wxEXPAND, 5 );
	
	ServantScrollingWindow *servant_names = new ServantScrollingWindow( this, false );
	m_scrollwin_names = servant_names;
	//m_scrollwin_names = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxVSCROLL );
	//m_scrollwin_names = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_scrollwin_names->SetScrollRate( 0, 5 );

	// Sizer for the channel names
	m_bSizer_names = new wxBoxSizer( wxVERTICAL );
	
	m_scrollwin_names->SetSizer( m_bSizer_names );
	m_scrollwin_names->Layout();
	m_bSizer_names->Fit( m_scrollwin_names );
	bSizer_left->Add( m_scrollwin_names, 1, wxEXPAND | wxALL, 5 );
	
	fgSizer_dialog->Add( bSizer_left, 1, wxEXPAND, 5 );
	
	//m_scrollwin_channels = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxHSCROLL|wxVSCROLL );
	ChannelScrollingWindow* channel_window = new ChannelScrollingWindow( this, m_scrollwin_timeline, m_scrollwin_names );
	m_scrollwin_channels = channel_window;
	m_scrollwin_channels->SetScrollRate( 5, 5 );
	//m_scrollwin_channels->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_APPWORKSPACE ) );
	m_scrollwin_channels->SetBackgroundColour( *wxLIGHT_GREY );
	
	//this->Connect( m_scrollwin_channels->GetId(), wxEVT_SCROLLWIN_THUMBTRACK,
	//				wxScrollWinEventHandler(chnlTraxDialog::ScrollChanged) );
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

	// Connect Events
	m_scrollwin_channels->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseDown ), NULL, this );
	m_scrollwin_channels->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseUp ), NULL, this );
	m_scrollwin_channels->Connect( wxEVT_MOTION, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseMove ), NULL, this );
	m_button_ZoomCenter->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_ZoomCenter_Click ), NULL, this );
	m_button_PrevTick->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_PrevTick_Click ), NULL, this );
	m_button_NextTick->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_NextTick_Click ), NULL, this );
	m_button_Delete->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Delete_Click ), NULL, this );
	m_button_Copy->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Copy_Click ), NULL, this );
	m_button_Paste->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Paste_Click ), NULL, this );
	m_button_Split->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Split_Click ), NULL, this );
	m_button_Move->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Move_Click ), NULL, this );
	m_button_Sel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_Sel_Click ), NULL, this );
	m_button_AddMarker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_AddMarker_Click ), NULL, this );
	m_button_RemoveMarker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_DeleteMarker_Click ), NULL, this );
	m_button_PrevMarker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_PrevMarker_Click ), NULL, this );
	m_button_NextMarker->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_NextMarker_Click ), NULL, this );
	m_button_AddNote->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_AddNote_Click ), NULL, this );
	m_button_RemoveNote->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_DeleteNote_Click ), NULL, this );
	m_button_PrevNote->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_PrevNote_Click ), NULL, this );
	m_button_NextNote->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( chnlTraxDialog::button_NextNote_Click ), NULL, this );
	
	//	set the trax editor to the current timeline max
	this->SetTimeRange(tmlnTimeLine::GetMinimum(), tmlnTimeLine::GetMaximum());

	// set up the time formats to the current settings
	this->UpdateTimeLabel();

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, 
		wxAuiPaneInfo().Name(title).Caption(title).Show().Layer(2).Bottom().MinimizeButton(true));

	// Register for hot keys
	twxMessaging::WindowWantsHotKeys(this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlTraxDialog::~chnlTraxDialog()
{
	// Disconnect Events
	//m_scrollwin_channels->Disconnect(wxEVT_PAINT, wxPaintEventHandler(chnlTraxDialog::OnPaint), NULL, this);
	m_scrollwin_channels->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseDown ), NULL, this );
	m_scrollwin_channels->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseUp ), NULL, this );
	m_scrollwin_channels->Disconnect( wxEVT_MOTION, wxMouseEventHandler( chnlTraxDialog::ChannelPanel_MouseMove ), NULL, this );

	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing chnlTraxDialog()");
	if (chnlTraxDialog::FormInstance == this)
		chnlTraxDialog::FormInstance = NULL;

	chnlOperations::ClearMonitors();
}

//--------------------------------------------------------------------
// Update channels based on selection
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateChannels()
{
	m_scrollwin_channels->Freeze();
	m_scrollwin_names->Freeze();
	setup_display(tmlnSelectionUtil::GetSelectedScriptObject());
	m_scrollwin_channels->Thaw();
	m_scrollwin_names->Thaw();

	center_view();

	adjust_scrollbars(m_scrollwin_channels);
	adjust_scrollbars(m_scrollwin_names);
}
void chnlTraxDialog::Select(tmlnScriptObject* i_pObject)
{
	m_scrollwin_channels->Freeze();
	m_scrollwin_names->Freeze();
	setup_display(i_pObject);
	m_scrollwin_channels->Thaw();
	m_scrollwin_names->Thaw();

	center_view();

	adjust_scrollbars(m_scrollwin_channels);
	adjust_scrollbars(m_scrollwin_names);
}

//--------------------------------------------------------------------
// Remove all channel  controls from form
//--------------------------------------------------------------------
void chnlTraxDialog::ClearChannels()
{
	chnlOperations::ClearMonitors();

	m_Channels.clear();

	const bool bDeleteAll = true;
	m_bSizer_names->Clear(bDeleteAll);
	m_bSizer_channels->Clear(bDeleteAll);
	m_staticText2->SetLabel(wxT(""));

}

//--------------------------------------------------------------------
// SetTimeRange changes length of timeline controls, setting
//	 minimum and maximum time range.
//--------------------------------------------------------------------
void chnlTraxDialog::SetTimeRange(const maTime& i_MinTime, const maTime& i_MaxTime)
{
	//if (i_Time > 0)
	{
		//TIME - should the wxWidgets time controls use seconds or maTime?
		float min_time = i_MinTime.AsSeconds();
		float max_time = i_MaxTime.AsSeconds();
		this->m_timeLabel->SetTimeRange( min_time, max_time );
		this->m_timeSlider->SetTimeRange( min_time, max_time );
		this->m_markerBar->SetTimeRange( min_time, max_time );

		// Set total time for channels also
		std::vector<chnlChannelControl*>::iterator it;
		for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
			(*it)->SetTimeRange( min_time, i_MaxTime.AsSeconds() );

		chnlTimelineGuideUtil::SetMinTime( i_MinTime );

		// Everytime range of timeline is changed (i.e. file load)
		// the timeline is scaled so that the full timeline is visible
		this->CalculateMaxZoom();

		resize_scrolled_windows();
	}
}

//----------------------------------------------------------------------------
// Zoom time scale so that full timeline is visible
//----------------------------------------------------------------------------
void chnlTraxDialog::CalculateMaxZoom()
{
	// Calculate a zoom value such that the full timeline is visible
	float totaltime = (tmlnTimeLine::GetMaximum() - tmlnTimeLine::GetMinimum()).AsSeconds();
	const int c_PanelEdgeSize = 50; // small amount of pixels to shrink to avoid scrollbars
	int panel_width = m_scrollwin_timeline->GetSize().GetWidth() - c_PanelEdgeSize;
	
	float zoom = panel_width / totaltime;
	maFunctions::Clamp(zoom, m_slider_zoom->GetMinimum(), m_slider_zoom->GetMaximum());
	this->m_slider_zoom->SetValue( zoom );
	this->SetTimeScale( zoom );
}

//--------------------------------------------------------------------
//	Reset the scale.  This is important when the user changes the
//	global frame rate and the channel drivers need to resize
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateScale()
{
	float zoom = this->m_slider_zoom->GetValue();
	this->SetTimeScale( zoom );
}

float chnlTraxDialog::GetSliderZoom()
{
	return this->m_slider_zoom->GetValue();
}

void chnlTraxDialog::SetSliderZoom(float zoom)
{
	this->m_slider_zoom->SetValue(zoom);
}
//--------------------------------------------------------------------
// TimeScale changes the zoom left/right
//--------------------------------------------------------------------
void chnlTraxDialog::SetTimeScale(float i_Scale)
{
	float new_scale = i_Scale;

	//	adjust the value for the frame-rate
	//new_scale = new_scale * (g3dConstants::c_fDefaultFrameRate / 24.0f);

	if (new_scale > 0)
	{
		this->m_timeLabel->SetTimeScale( new_scale );
		this->m_timeSlider->SetTimeScale( new_scale );
		this->m_markerBar->SetTimeScale( new_scale );

		// Set time scale for channels also
		std::for_each(m_Channels.begin(), m_Channels.end(),
			std::bind(std::mem_fn(&chnlChannelControl::SetTimeScale),
						 std::placeholders::_1, new_scale));

		chnlTimelineGuideUtil::SetTimeScale( new_scale );
		m_scrollwin_channels->Refresh();

		//	adjust the timeline position based on the selected object or current time.
		//
		center_view();

		resize_scrolled_windows();
	}
}

//--------------------------------------------------------------------
// SetSnapInterval alters grid unit for moving drivers in channels
//--------------------------------------------------------------------
void chnlTraxDialog::SetSnapInterval(int i_Interval)
{
	m_SnapInterval = i_Interval;
	std::for_each(m_Channels.begin(), m_Channels.end(),
		std::bind(std::mem_fn(&chnlChannelControl::SetSnapInterval),
					 std::placeholders::_1, i_Interval));
}
//--------------------------------------------------------------------
//  This driver has changed its properties related to the
//	trax editor display, so update the channels related to it.
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateDriver(tmlnDriver *i_pDriver)
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);
		const int num_clips = pChannel->GetNumClips();
		for (int i=0; i<num_clips; ++i)
		{
			chnlClipDriver* pClip = dynamic_cast<chnlClipDriver*>(pChannel->GetClip(i));
			if (pClip)
			{
				if (&pClip->Driver() == i_pDriver)
				{
					pClip->Update();

					// invalidate the control in order to force a redraw soon
					pChannel->Refresh();

					// Note, driver can occur in more than one channel, 
					// but only once per channel. So, one level "break" 
					// is used here instead of a "return"
					break;
				}
			}
		}
	}

}

//--------------------------------------------------------------------
// Time format has changed, alter display
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateTimeLabel()
{
	// Check the format in the timeline label
	m_timeLabel->SetFramesPerSecond( tmlnTimeLine::GetFPS() );

	bool bFrameTimeFormat = (tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_Frames);
	m_timeLabel->SetDisplayFrames(bFrameTimeFormat);
	m_timeLabel->SetDisplayMinutes((tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_HHMMSSFR)
								 || (tmlnTimeLine::GetTimeFormat() == tmlnTimeLine::e_HHMMSSMS));

	//	update the title of the panel with the time string
	//
	int frames;
	tmlnTimeUtil::GetTimeInFrames(tmlnTimeLine::GetValue(), frames);

	std::ostringstream str;
	if (bFrameTimeFormat)
	{
		// Just display frames
		str << "[" << frames << "]";
	}
	else
	{
		// Display time format and frames for convenience
		std::string time_string;
		tmlnTimeUtil::GetTimeString(tmlnTimeLine::GetValue(), time_string);
		str << time_string << "[" << frames << "]";
	}
	twxPaneMgr::SetCaption( this, str.str() );
	
	// Check the snap interval, if it is attached to the current FPS
	if (m_checkBox_Snap->GetValue())
		this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
}

//--------------------------------------------------------------------
// Timeline time has changed, alter display
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateCurrentTime()
{
	// set the time
	this->m_timeSlider->SetCurrentTime( tmlnTimeLine::GetValue().AsSeconds() );

	//	update the Time GuideLine
	chnlTimelineGuideUtil::SetCurrentTime( tmlnTimeLine::GetValue() );
	m_scrollwin_channels->Refresh();

	UpdateTimeLabel();
}

//--------------------------------------------------------------------
// Look for time ticks based on begin and end times of drivers
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateTimeTicks()
{
	this->m_timeSlider->ClearTimeTicks();

	const std::set<maTime>& time_ticks = chnlTimeTickMgr::GetTimeTicks();
	std::set<maTime>::const_iterator it;
	for (it = time_ticks.begin(); it != time_ticks.end(); ++it)
	{
		this->m_timeSlider->AddTimeTick(it->AsSeconds());	//TIME - time ticks in GUI should be seconds?
	}
}

//--------------------------------------------------------------------
// Update marker bar's display of markers and notes
//--------------------------------------------------------------------
void chnlTraxDialog::UpdateMarkersAndNotes()
{
	this->m_markerBar->ClearMarkers();
	this->m_markerBar->ClearNotes();

	//	Markers
	for (int i=0; i < chnlMarkerMgr::GetNumMarkers(); ++i)
	{
		const chnlMarkerDataItem& mdata = chnlMarkerMgr::GetMarkerData(i);
		this->m_markerBar->AddMarker( mdata.m_Time.GetValue().AsSeconds(), 
									(chnlMarkerIcon::MarkerType) mdata.m_TimeMarkerType.GetValue(),
									 mdata.m_Note.GetValue() );
	}

	//	Notes
	for (int i=0; i < chnlNotesMgr::GetNumNotes(); ++i)
	{
		const chnlNoteDataItem& ndata = chnlNotesMgr::GetNoteData(i);
		this->m_markerBar->AddNote( ndata.m_Time.GetValue().AsSeconds(), 
									 (chnlNoteIcon::NoteStatus) ndata.m_Status.GetValue(),
									 ndata.m_Note.GetValue().c_str() );
	}
}

//------------------------------------------------------------------------
// Get set of drivers that are selected in the trax editor. If skip locked
// is true, then only drivers in channels that are unlocked will be returned.
//------------------------------------------------------------------------
void chnlTraxDialog::GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, 
										bool i_bSkipLocked)
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);

		//	don't allow this interaction if the channel is locked
		if (!i_bSkipLocked || !pChannel->GetLocked())
		{
			std::transform(pChannel->GetSelectedClips().begin(), 
							pChannel->GetSelectedClips().end(),
							std::inserter(o_Drivers, o_Drivers.begin()),
							get_driver_for_clip);
		}				
	}
}

//------------------------------------------------------------------------
// Deselect all drivers in the channel interface
//------------------------------------------------------------------------
void chnlTraxDialog::ClearSelection()
{
	std::for_each(m_Channels.begin(), m_Channels.end(), 
		std::mem_fn(&chnlChannelControl::ClearSelection));
}

//------------------------------------------------------------------------
// Select driver in the channel interface - this will do an append
//	to the selection, it will not deselect other drivers.
//------------------------------------------------------------------------
void chnlTraxDialog::AddToSelection( tmlnDriver* i_pDriver )
{
	const int num_channels = m_Channels.size();
	for (int i=0; i<num_channels; i++)
	{	
		for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
		{
			chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
			if (clip)
			{
				if (&clip->Driver() == i_pDriver)
				{
					const bool append_selection = true;
					m_Channels[i]->SelectClip(clip, append_selection);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// Interaction Callbacks from channel controls
//--------------------------------------------------------------------
//virtual 
void chnlTraxDialog::ClipSelected(chnlChannelControl* i_pChannel, chnlChannelClip* i_pClip, bool i_bAppended)
{
	tmlnDriver *pEditingDriver = NULL;
	if (i_pClip)
	{
		// Got selected clip
		chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (clip)
			pEditingDriver = (&clip->Driver());
	}

	const int num_channels = m_Channels.size();
	// If we are clearing the selection, or not appending to 
	// the selection, then clear selection from other channels
	if (i_pClip == NULL || i_bAppended == false)
	{
		for (int i=0; i<num_channels; i++)
		{	
			if (m_Channels[i] != i_pChannel)
				m_Channels[i]->ClearSelection();
		}
		
		//	Clear out the driver dialog, if needed
		tmlnDriverDialogUtil::PrepareForSelection(pEditingDriver);
	}

	// Now, look for all other occurrences of this driver on other channels
	if (i_pClip != NULL)
	{
		chnlClipDriver* sel_clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (sel_clip)
		{
			tmlnDriver *pSelDriver = &sel_clip->Driver();
			for (int i=0; i<num_channels; i++)
			{	
				if (m_Channels[i] != i_pChannel)
				{
					for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
					{
						chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
						if (clip)
						{
							if (&clip->Driver() == pSelDriver)
							{
								const bool append_selection = true;
								m_Channels[i]->SelectClip(clip, append_selection);
							}
						}
					}
				}
			}
		}
	}


	// Prepare all selected clips in all channels for the interaction
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fn(&chnlChannelControl::BeginInteraction));

	// Update the properties dialog for the selected driver
	// (Should this merge all driver properties together like with objects?)
	if (pEditingDriver && guiDialogTabbedMgr::IsVisible("Driver"))
	{
		// Got selected clip
		pEditingDriver->DoEditProperties();
	}
}
void chnlTraxDialog::ClipDeselected(chnlChannelControl* i_pChannel, chnlChannelClip* i_pClip)
{
	const int num_channels = m_Channels.size();

	// Deselect all occurrences of this driver on other channels
	if (i_pClip != NULL)
	{
		chnlClipDriver* sel_clip = dynamic_cast<chnlClipDriver*>(i_pClip);
		if (sel_clip)
		{
			tmlnDriver *pSelDriver = &sel_clip->Driver();
			for (int i=0; i<num_channels; i++)
			{	
				if (m_Channels[i] != i_pChannel)
				{
					for (int c=0; c<m_Channels[i]->GetNumClips(); c++)
					{
						chnlClipDriver* clip = dynamic_cast<chnlClipDriver*>(m_Channels[i]->GetClip(c));
						if (clip)
						{
							if (&clip->Driver() == pSelDriver)
							{
								m_Channels[i]->DeselectClip(clip);
							}
						}
					}
				}
			}
		}
	}

	//	Clear out the driver dialog
	tmlnDriverDialogUtil::ClearDriverProperties();		
}
//virtual 
void chnlTraxDialog::ClipMoved(chnlChannelControl*, float i_TimeDelta)
{
	//TIME - wxGUI time controls could use maTime
	maTime time_delta = maTime::FromSeconds( i_TimeDelta );

	//bool canBemoved = true;
	const int num_channels = m_Channels.size();
	std::vector<chnlChannelControl*> m_SelectedChannel;
	int num_conflicts = 0;
	for (int i=0; i<num_channels; i++)
	{
		
		std::set<chnlChannelClip*> selectedClips = m_Channels[i]->GetSelectedClips();
		if(selectedClips.size() > 0) m_SelectedChannel.push_back(m_Channels[i]);
		std::set<chnlChannelClip*>::iterator it;
		for (it = selectedClips.begin(); it!= selectedClips.end(); ++it )
		{
			chnlChannelClip* clip = (*it);
			if((clip->GetBeginTime() + time_delta )  < tmlnTimeLine::GetMinimum()
				|| (clip->GetBeginTime() + time_delta) > tmlnTimeLine::GetMaximum())
			return;
		}
	}

	for( int cha = 0; cha < m_Channels.size(); cha ++ )
	{
		std::set<chnlChannelClip*> selectedClips = m_Channels[cha]->GetSelectedClips();
		std::set<chnlChannelClip*>::iterator it;
		for (it = selectedClips.begin(); it!= selectedClips.end(); ++it )
		{
			chnlChannelClip * clip = (*it);
			num_conflicts += check_channel_for_driver_conflicts(m_Channels[cha],clip, time_delta );
		}
	}
	
	show_driver_conflict_error(num_conflicts);
	for(int m = 0 ; m < m_SelectedChannel.size() ; m++)
	{
		if(m_SelectedChannel[m]->GetLocked())
		{
			return; // Even if one channel is locked, abstain from moving any other channels
		}
	}
	// Slide all selected clips in all channels for the interaction
	std::for_each( m_Channels.begin(), m_Channels.end(), 
		std::bind(std::mem_fn(&chnlChannelControl::InteractMoveSelectedClips),
					 std::placeholders::_1, i_TimeDelta));
	

	// Adjust the selected clips to align with time ticks and markers
	if (PrefsMgr::Data().m_ChannelEditor_SnapActive.GetValue())
	{
		std::for_each(m_Channels.begin(), m_Channels.end(), align_selected_clips);
		m_scrollwin_channels->Refresh();
	}
}
//virtual 
void chnlTraxDialog::ClipMoveFinished(chnlChannelControl*)
{
	int num_conflicts = check_channel_for_driver_conflicts();
	show_driver_conflict_error(num_conflicts);
	
	// End interaction in all channels
	std::for_each(m_Channels.begin(), m_Channels.end(), 
					std::mem_fn(&chnlChannelControl::FinishInteraction));
	chnlTimelineGuideUtil::HideMarkerGuide();
	m_scrollwin_channels->Refresh();

	
}
//virtual 
void chnlTraxDialog::ClipResized(chnlChannelControl*)
{
	
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::ChannelPanel_MouseDown(wxMouseEvent& i_Event)
{
	//DBG_LOG("channel editor general mouse down");

	m_bDriverSelectStart = true;
	m_bleftIsDown = true;
	m_InitX = i_Event.m_x;
	m_InitY = i_Event.m_y;
	savePoint.x = m_InitX;
	windowWidth = this->GetSize();
	savePoint.y = m_InitY;
	m_DriverSelectStartX = i_Event.m_x;
	m_DriverSelectStartY = i_Event.m_y;
	m_DriverSelectStartTime = get_time_from_point( m_DriverSelectStartX, m_DriverSelectStartY );
	//chnlTimelineMarquee::SetInitPos(m_InitX, m_InitY);
	i_Event.Skip();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void chnlTraxDialog::ChannelPanel_MouseMove(wxMouseEvent& i_Event)
{
	if (m_bDriverSelectStart)
	{
		//Refresh();
		//DBG_LOG("channel editor general mouse up");
		//
		////select_clips( m_DriverSelectStartX, m_DriverSelectStartY, e->X, e->Y );
		bool shiftIsPressed = i_Event.ShiftDown();
	//	wxPoint temp(0,0);
	//	temp = ::wxGetMousePosition();
	//	wxWindowDC pdc(::wxFindWindowAtPoint(temp)) ;
	//	wxPen pen(*wxRED,3,wxDOT);
	//	wxDC &dc = pdc;
	//	
	//	chnlTimelineMarquee::SetIsMarquee(true);
	//	
	////	chnlTimelineMarquee::DrawAllMarquee(i_Event.m_x, i_Event.m_y);
	//	dc.SetBrush(*wxBLUE_BRUSH);
	//	dc.SetPen(pen);
	//	width = savePoint.x - m_InitX;
	//	height = savePoint.y - m_InitY;
	//	dc.SetLogicalFunction(wxINVERT);	
	//	
	//	dc.DrawRectangle(m_InitX, m_InitY , width, height);
	//	//this->ScreenToClient(&endPoint.x, &endPoint.y);
	//	endPoint.x = i_Event.m_x;
	//	endPoint.y = i_Event.m_y;
	//	width = endPoint.x - m_InitX;
	//	height = endPoint.y - m_InitY;
	//	dc.DrawRectangle(m_InitX , m_InitY , width, height);
	//	dc.SetLogicalFunction(wxCOPY);
	//	savePoint = endPoint;
		float curr_time = get_time_from_point( i_Event.m_x, i_Event.m_y );
		select_clips_within_time( m_DriverSelectStartTime, curr_time, shiftIsPressed );

		//DBG_LOG2("   start time %6.3f  curr time %6.3f", m_DriverSelectStartTime, curr_time);
		i_Event.Skip();
	}
	
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::ChannelPanel_MouseUp(wxMouseEvent& i_Event)
{
	//DBG_LOG("channel editor general mouse up");
	//chnlTimelineMarquee::SetIsMarquee(false);
	m_bDriverSelectStart = false;
	savePoint.x = i_Event.m_x;
	savePoint.y = i_Event.m_y;
	
	Refresh();
	i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::TimeChanged(wxCommandEvent &i_Event)
{
	current_time_changed();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::ZoomChanged(wxCommandEvent &i_Event)
{
	this->SetTimeScale( m_slider_zoom->GetValue() );
}

//------------------------------------------------------------------------
// combo box of filters changed
//------------------------------------------------------------------------
void chnlTraxDialog::FilterIndexChanged(wxCommandEvent& i_Event)
{
	this->m_nFilterState = this->m_choice_Display->GetSelection();
	this->UpdateChannels();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::checkBox_Snap_Clicked(wxCommandEvent& i_Event)
{
	 if (this->m_checkBox_Snap->GetValue())
		 this->SetSnapInterval((int)tmlnTimeLine::GetFPS());
	 else
		 this->SetSnapInterval(c_DefaultSnapInterval);
}

//----------------------------------------------------------------------------
// button events
//----------------------------------------------------------------------------
void chnlTraxDialog::button_ZoomCenter_Click( wxCommandEvent& i_Event )
{
	this->CalculateMaxZoom();
}

void chnlTraxDialog::button_PrevTick_Click( wxCommandEvent& i_Event )
{
	chnlOperations::MoveTimePrevTick();
}
void chnlTraxDialog::button_NextTick_Click( wxCommandEvent& i_Event )
{
	chnlOperations::MoveTimeNextTick();
}
void chnlTraxDialog::button_Delete_Click( wxCommandEvent& i_Event )
{
	chnlOperations::DeleteSelectedDrivers();
}
void chnlTraxDialog::button_Copy_Click( wxCommandEvent& i_Event )
{
	chnlOperations::CopyDrivers();
}
void chnlTraxDialog::button_Paste_Click( wxCommandEvent& i_Event )
{
	chnlOperations::PasteDrivers();
}
void chnlTraxDialog::button_Split_Click( wxCommandEvent& i_Event )
{
	chnlOperations::SplitDrivers();
}
void chnlTraxDialog::button_Move_Click( wxCommandEvent& i_Event )
{
	if (any_drivers_selected())
	{
		prtyFloat moveAmount(tmlnTimeUtil::GetTimeFormatString(), 0.0f);
		moveAmount.AddCallback(new prtyCallbackWrapper<chnlTraxDialog>(this, &chnlTraxDialog::MoveAmountChanged));

		prtyPropertyUIInfoContainer moveInfo;
		moveInfo.Add(new tmlnTimeEditUIInfo(&moveAmount));

		// Start interaction, preparing the selected drivers to move delta amounts
		std::for_each(m_Channels.begin(), m_Channels.end(), 
			std::mem_fn(&chnlChannelControl::BeginInteraction));

		if (guiPropertyDialog::ShowModal("Move Drivers", 
										 moveInfo, 
										 "Time amount to move drivers:") != guiPropertyDialog::e_OK)
		{
			// If the user pressed "Cancel", then put the drivers back.
			// Our property callback is still active, so all we have to do is set 
			// the move amount to 0
			moveAmount.SetValue(0.0f); 
		}
		
		// Finish interaction, setting deltas into drivers
		std::for_each(m_Channels.begin(), m_Channels.end(), 
			std::mem_fn(&chnlChannelControl::FinishInteraction));
	}
}
//--------------------------------------------------------------------
// Move driver dialog's time amount changes, set the
// delta amount into the drivers.
//--------------------------------------------------------------------
void chnlTraxDialog::MoveAmountChanged(prtyProperty *i_pProperty, bool i_bDirty)
{	
	prtyFloat *pMoveAmount = dynamic_cast<prtyFloat*>(i_pProperty);
	DBG_ASSERT(pMoveAmount, "Move amount callback should be a float property");
	 
	std::for_each(m_Channels.begin(), m_Channels.end(),
		std::bind(std::mem_fn(&chnlChannelControl::InteractMoveSelectedClips),
					 std::placeholders::_1, pMoveAmount->GetValue()));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chnlTraxDialog::button_Sel_Click( wxCommandEvent& i_Event )
{
	std::for_each(m_Channels.begin(), m_Channels.end(), 
		std::mem_fn(&chnlChannelControl::SelectClipsAfter));
}
void chnlTraxDialog::button_AddMarker_Click( wxCommandEvent& i_Event )
{
	chnlMarkerOperations::AddMarker();
}
void chnlTraxDialog::button_DeleteMarker_Click( wxCommandEvent& i_Event )
{
	chnlMarkerOperations::DeleteMarker();
}
void chnlTraxDialog::button_PrevMarker_Click( wxCommandEvent& i_Event )
{
	chnlMarkerOperations::MoveToPrevMarker();
}
void chnlTraxDialog::button_NextMarker_Click( wxCommandEvent& i_Event )
{
	chnlMarkerOperations::MoveToNextMarker();
}
void chnlTraxDialog::button_AddNote_Click( wxCommandEvent& i_Event )
{
	chnlNotesOperations::AddNote();
}
void chnlTraxDialog::button_DeleteNote_Click( wxCommandEvent& i_Event )
{
	chnlNotesOperations::DeleteNote();
}
void chnlTraxDialog::button_PrevNote_Click( wxCommandEvent& i_Event )
{
	chnlNotesOperations::MoveToPrevNote();
}
void chnlTraxDialog::button_NextNote_Click( wxCommandEvent& i_Event )
{
	chnlNotesOperations::MoveToNextNote();
}

//----------------------------------------------------------------------------
// Display channels for objects based on the filter state
//----------------------------------------------------------------------------
void chnlTraxDialog::setup_display(tmlnScriptObject* i_pObject)
 {
	switch (m_nFilterState)
	{
		case 0:
			// Show all selected objects
			multiple_selected_display();
			break;
		case 1:
			// Single selected object display
			single_object_display(i_pObject);
			break;
		case 2:
			// Show system objects
			system_object_display(i_pObject);
			break;
		case 3:
			// Show all objects
			all_object_display();
			break;
	}
}

//----------------------------------------------------------------------------
// Display channels for all selected objects
//----------------------------------------------------------------------------
void chnlTraxDialog::multiple_selected_display()
{
	this->ClearChannels();
	
	std::vector<tmlnScriptObject*> objects;
	tmlnSelectionUtil::GetSelectedScriptObjects(objects);

	for (int i=0; i < objects.size(); i++)
	{
		add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	check_channel_for_driver_conflicts();
	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for the selected object only
//----------------------------------------------------------------------------
void chnlTraxDialog::single_object_display(tmlnScriptObject* i_pObject)
{
	this->ClearChannels();
	

	add_channels_for_object(i_pObject);
	check_channel_for_driver_conflicts();
	//this->labelNoChannels->Visible = (i_pObject == NULL);

	chnlTimeTickMgr::GatherTimeTicks(i_pObject);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for all objects of the same type as the given object
//----------------------------------------------------------------------------
void chnlTraxDialog::system_object_display(tmlnScriptObject* i_pObject)
{
	this->ClearChannels();

	std::vector<tmlnScriptObject*> objects;

	if (i_pObject != NULL)
	{
		tmlnTimelineMgr::GetObjects( objects, tmlnTimelineMgr::GetObjectCategory(i_pObject) );
	}

	for (int i=0; i < objects.size(); i++)
	{
		// In "system" display, only add objects with drivers
		if (objects[i]->GetNumDrivers() > 0)
			add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}

//----------------------------------------------------------------------------
// Display channels for all script objects
//----------------------------------------------------------------------------
void chnlTraxDialog::all_object_display()
{
	this->ClearChannels();

	std::vector<tmlnScriptObject*> objects;
	tmlnTimelineMgr::GetObjects( objects );
	for (int i=0; i < objects.size(); i++)
	{
		// In "all object" display, only add objects with drivers
		if (objects[i]->GetNumDrivers() > 0)
			add_channels_for_object(objects[i]);
	}

	//this->labelNoChannels->Visible = (objects.empty());

	chnlTimeTickMgr::GatherTimeTicks(objects);
	this->UpdateTimeTicks();
}
 
//----------------------------------------------------------------------------
// Add channels for the given object to our display
//----------------------------------------------------------------------------
void chnlTraxDialog::add_channels_for_object(tmlnScriptObject* i_pObject)
{
	if (!i_pObject) return;

	// Add Category based on object's name
	this->add_category(itString(i_pObject->GetDisplayName().c_str()));

	// Add Channels for object that have a driver ONLY
	//
	tmlnChannelSet& chnl_set = i_pObject->ChannelSet();
	int num_channels = chnl_set.GetNumChannels();
	for (int i=0; i<num_channels; i++)
	{
		tmlnChannel& channel = chnl_set.Channel(i);

		//	if drivers for channel, then add it.
		//
		if ( channel.GetNumDrivers() > 0 )
		{
			chnlChannelControl *pChannelCtrl = this->add_channel(channel);
				//this->add_channel(channel.GetName(), channel.IsLocked() );
			//pChannelCtrl->MultiChannel = channel.IsMultiChannel();
			
			for (int c=0; c<channel.GetNumDrivers(); c++)
			{
				tmlnDriver& driver = channel.Driver(c);

				chnlClipDriver *pClip = new chnlClipDriver(	driver,
															driver.GetName(),
															driver.GetBeginTime(), 
															driver.GetEndTime() );
				//pClip->Category = gcnew System::String(driver.GetCategory().c_str());

				maFloatRGBA color = driver.GetClipFillColor();
				pClip->SetFillColor(color.GetRed(), 
									color.GetGreen(), 
									color.GetBlue());

				pChannelCtrl->AddClip( pClip );

				chnlOperations::MonitorChanges(driver);
			}
		}
	}
}

//--------------------------------------------------------------------
// Add category label to form
//--------------------------------------------------------------------
void chnlTraxDialog::add_category(const itString& i_CategoryName)
{
	wxStaticText *pCategory = new wxStaticText( m_scrollwin_names, wxID_ANY, i_CategoryName.GetString(), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
	pCategory->Wrap( -1 );
	pCategory->SetFont(wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ));
	pCategory->SetMinSize(FromDIP(wxSize(-1, 20)));
	pCategory->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
	pCategory->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
	m_bSizer_names->Add( pCategory, 0, wxEXPAND, 5 );

	// Add spacer on channel side of controls
	m_bSizer_channels->Add(0, 20, 0, wxEXPAND, 5);
}

//--------------------------------------------------------------------
// Add channel control and label to form
//--------------------------------------------------------------------
chnlChannelControl* chnlTraxDialog::add_channel(tmlnChannel& i_Channel)
{	
	
	wxString icon_dir = ::get_icon_directory();
	// Compute an alternating color for the channels
	//bool channel_alt = (m_bSizer_names->GetItems().size() % 2 == 1);
	bool channel_alt = (m_Channels.size() % 2 == 1);
	wxColor bg_color;
	if (channel_alt)
	{
		bg_color.Set(250, 245, 230);
	}
	else
	{
		bg_color.Set(255, 255, 255);
	}


	// Paenl holds spacer, channel name label, check box for locked
	wxPanel* pNamePanel = new wxPanel( m_scrollwin_names );
	pNamePanel->SetBackgroundColour(bg_color);
	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );
	//wxStaticBoxSizer* bSizer = new wxStaticBoxSizer( wxHORIZONTAL, m_scrollwin_names );
	bSizer->SetMinSize( FromDIP(wxSize( -1, 32 )) ); 
	bSizer->Add( 10, 0, 0, wxEXPAND, 5 ); //spacer
	// Channel name label
	wxStaticText *pLabel = new wxStaticText( pNamePanel, wxID_ANY, 
		wxString(i_Channel.GetName().c_str(), wxConvUTF8), wxDefaultPosition, wxDefaultSize, 0 );
	pLabel->SetFont(wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ));
	pLabel->Wrap( -1 );
	bSizer->Add( pLabel, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	
    wxLogNull nullLog;

    //wxBitmap *pBitmapUnlock = new wxBitmap(icon_dir + wxT("\\padlock_disabled.png"), wxBITMAP_TYPE_ANY);
	wxBitmap pBitmapUnlock(icon_dir + wxT("\\padlock_disabled.png"), wxBITMAP_TYPE_ANY);
	wxBitmap pBitmapLock(icon_dir + wxT("\\padlock.png"), wxBITMAP_TYPE_ANY);
	wxString name(wxT("Bitmap Button"));

	// Check box for locked state of channel
	ChannelLockedBitmapButton* pLocked = new ChannelLockedBitmapButton( pNamePanel, i_Channel, pBitmapUnlock,pBitmapLock,  name );
	bSizer->Add( pLocked, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	pNamePanel->SetSizer(bSizer);
	// Add to sizer in scrolled window on left
	m_bSizer_names->Add( pNamePanel, 0, wxEXPAND, 5 );
	m_scrollwin_names->Layout();

	// Add channel control to main scrolled window in lower right
	chnlChannelControl *pChannelCtrl = new chnlChannelControl( m_scrollwin_channels, i_Channel.IsLocked() );
	pChannelCtrl->SetBackgroundColour(bg_color);
	m_bSizer_channels->Add( pChannelCtrl, 0, wxLEFT|wxRIGHT, 0 );
	pChannelCtrl->SetTimeScale( this->m_timeLabel->GetTimeScale() );
	pChannelCtrl->SetTimeRange(	m_timeLabel->GetMinTime(), m_timeLabel->GetMaxTime() );
	pChannelCtrl->SetSnapInterval( this->m_SnapInterval );
	m_scrollwin_channels->Layout();

	pChannelCtrl->SetInteractionCallback(this);
	m_Channels.push_back(pChannelCtrl);

	// Associate check box with the channel control to maintain locked state
	pLocked->AssignChannelControl(pChannelCtrl);
	pLocked->SetBitmaps(pBitmapUnlock, pBitmapLock);
	return pChannelCtrl;
}

//----------------------------------------------------------------------------
// This will call ::Layout() and ::AdjustScrollbars()
// for the scrolled windows:
//----------------------------------------------------------------------------
void chnlTraxDialog::resize_scrolled_windows()
{
    wxSize size = m_scrollwin_timeline->GetBestVirtualSize();
    m_scrollwin_timeline->SetVirtualSize( size );
    size = m_scrollwin_channels->GetBestVirtualSize();
    m_scrollwin_channels->SetVirtualSize( size );
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::current_time_changed()
 {
	float time = this->m_timeSlider->GetCurrentTime();
	//float origtime = time;

	tmlnTimeUtil::AdjustTimeToFrame( time );
	//if (time != origtime)

	// the "Deferred" call will set the timeline after the
	// current render thread finishes
	tmlnTimeLine::SetTimeDeferred( maTime::FromSeconds(time) );
 }

//----------------------------------------------------------------------------
// Return true if any drivers are selected in any channels
//----------------------------------------------------------------------------
bool chnlTraxDialog::any_drivers_selected()
{
	std::vector<chnlChannelControl*>::iterator it;
	for (it = m_Channels.begin(); it != m_Channels.end(); ++it)
	{
		chnlChannelControl *pChannel = (*it);
		if (!pChannel->GetSelectedClips().empty())
			return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::center_view()
{
	// could center on current time or on selected clips
	center_view_on_currenttime();
}

//----------------------------------------------------------------------------
//	adjust the timeline position based on the current time.
//----------------------------------------------------------------------------
void chnlTraxDialog::center_view_on_currenttime()
{
	//System::Drawing::Point newpoint;
	int time_pos = this->m_timeSlider->GetCurrentTimePosition();
	int width = m_scrollwin_timeline->GetClientSize().GetWidth();
	//int cur_pos = 5 * m_scrollwin_timeline->GetScrollPos(wxHORIZONTAL);

	int centered_pos = time_pos - (width / 2);
	if (centered_pos < 0) centered_pos = 0;

	// Only alter scroll if the time position is outside of the center half of the timeline
	//if (::abs(centered_pos - cur_pos) > width / 4)
	m_scrollwin_timeline->Scroll(centered_pos/5, 0); // scrolls in units of 5

}

//----------------------------------------------------------------------------
// Calculate the time based on the point given
//----------------------------------------------------------------------------
float chnlTraxDialog::get_time_from_point(int i_CurrentX, int i_CurrentY)
{
	//DBG_LOG2("panel loc   (%d,%d)", panel_channels->Location.X, panel_channels->Location.Y);
	//DBG_LOG2("panel aspos (%d,%d)", panel_channels->AutoScrollPosition.X, panel_channels->AutoScrollPosition.Y);
	//DBG_LOG2("panel asoff (%d,%d)", panel_channels->AutoScrollOffset.X, panel_channels->AutoScrollOffset.Y);
	//DBG_LOG2("mouseclick  (%d,%d)", i_CurrentX, i_CurrentY);

	// Consider scrolling ..
    int stepx, stepy;
    m_scrollwin_channels->GetScrollPixelsPerUnit(&stepx, &stepy);
    int startx, starty;
    m_scrollwin_channels->GetViewStart(&startx, &starty);

	int x_offset = i_CurrentX + (stepx*startx);
	return m_timeSlider->GetTimeAtPosition( x_offset );
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTraxDialog::select_clips_within_time( float i_StartTime, float i_CurrentTime, bool shiftIsPressed )
{
	//	loop through the channels and driver clips and select all the ones that fall within the "box"
	//
	const int num_channels = m_Channels.size();
	float begin_time, end_time;
	if(!shiftIsPressed)
	{
		for (int i=0; i<num_channels; i++)
		{	
			m_Channels[i]->ClearSelection();
		}
	}

	if (i_StartTime <= i_CurrentTime)
	{
		begin_time = i_StartTime;
		end_time = i_CurrentTime;
	}
	else
	{
		begin_time = i_CurrentTime;
		end_time = i_StartTime;
	}

	
	for (int i=0; i<num_channels; i++)
	{	
		m_Channels[i]->SelectClips( begin_time, end_time );
	}
}
//----------------------------------------------------------------------------
//Driver Conflict functions
//----------------------------------------------------------------------------

void chnlTraxDialog::show_driver_conflict_error(int i_NumConflicts)
{
	if(i_NumConflicts ==0)
	{
		m_staticText2->SetLabel(wxT(""));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool chnlTraxDialog::check_driver_conflicts(chnlChannelClip* i_pClip1, const maTime& i_Delta, chnlChannelClip* i_pClip2, chnlChannelControl* i_pChannel ) const
{
	if(i_pClip1 == i_pClip2)
	{
		return false;
	}
	std::ostringstream oss;
	bool Overlaps = false;
	
	std::string driverName = i_pClip1->GetName();
	
	maTime c1s = i_pClip1->GetBeginTime()  + i_Delta;
	maTime c1e = i_pClip1->GetEndTime()  + i_Delta;
	maTime c2s = i_pClip2->GetBeginTime();
	maTime c2e = i_pClip2->GetEndTime();
	
	std::string time_str;
	if  ((c2s <= c1s) && (c2e > c1s))		// clip 2 start inside
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps = true;
	}
	else if ((c2s < c1e) && (c2e > c1e))		// clip 2 end inside
	{
		tmlnTimeUtil::GetTimeString(c1e, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	else if ((c1s < c2s) && (c1e > c2s))		// clip 1 start inside
	{
		tmlnTimeUtil::GetTimeString(c2s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	else if ((c1s < c2e) && (c1e > c2e))   	// clip 1 end inside
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps = true;
	}
	else if ((c1s == c2s) && ( c1e == c2e))  // keys overlap
	{
		tmlnTimeUtil::GetTimeString(c1s, time_str);
		oss << "Driver \""<< driverName << "\" conflicts at " << time_str;
		Overlaps =  true;
	}
	

	if(Overlaps)
	{
		m_staticText2->SetLabel( wxString::FromAscii(oss.str().c_str()));
		m_staticText2->SetForegroundColour(*wxRED);
		DBG_LOG(oss.str().c_str());
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts(chnlChannelControl *i_pChannel, chnlChannelClip* i_pClip, const maTime& i_Delta  ) const
{
	int clips_count = i_pChannel->GetNumClips();
	int num_conflicts = 0;
	for (int j=0; j < clips_count; j++)
	{
		chnlChannelClip* clip2 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(j));
		if(check_driver_conflicts(i_pClip, i_Delta, clip2, i_pChannel))
		{
			++num_conflicts;
		}
	}
	return num_conflicts;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts(chnlChannelControl* i_pChannel, const maTime& i_Delta) const
{
	int clips_count = i_pChannel->GetNumClips(); 
	int num_conflicts = 0;
	for (int i=0; i < clips_count; ++i)
	{
		chnlChannelClip* clip1 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(i));
		for (int j=i+1; j < clips_count; ++j)
		{
			chnlChannelClip* clip2 = dynamic_cast<chnlChannelClip*>(i_pChannel->GetClip(j));
			if(check_driver_conflicts(clip1, i_Delta, clip2, i_pChannel))
			{
				++num_conflicts;
			}
		}
	}
	return num_conflicts;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int chnlTraxDialog::check_channel_for_driver_conflicts() const
{
	int num_conflicts = 0;
	for(int i=0; i < m_Channels.size();i++)
	{
		chnlChannelControl* channel = dynamic_cast<chnlChannelControl*>(m_Channels[i]);
		num_conflicts +=check_channel_for_driver_conflicts(channel, maTime::c_ZeroTime);
	}
	return num_conflicts;
}
		

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//bool chnlTraxDialog::clip_within_box( ChannelClip^ i_Clip, int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY )
//{
//	//	How does this work in the window scrolls?  is it screen relative?
//	//
//	//				if (   ((i_Clip->xxx >= i_StartX) && (i_Clip->xxx <= i_CurrentX))
//	//					&& ((i_Clip->yyy >= i_StartY) && (i_Clip->yyy <= i_CurrentY)) )
//	//				{
//	//					return true;
//	//				}
//	return false;
//}
//void chnlTraxDialog::select_clips_within_box( int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY )
//{
//	// TODO - implement this function!!!
//
//	//	loop through the channels and driver clips and select all the ones that fall within the "box"
//	//
//	for (int i=0; i<this->channelList->Count; i++)
//	{	
//		ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
//
//		int count = 0;
//
//		//	loop through the channels
//		for (int i=0; i<this->channelList->Count; i++)
//		{	
//			ChannelControl^ channel = dynamic_cast<ChannelControl^>(channelList[i]);
//
//			//	loop through all the clips of the channels
//			int clips_count = channel->Clips->Count; 
//			for (int j=0; j < clips_count; ++j)
//			{
//				ChannelClip^ cclip = dynamic_cast<ChannelClip^>(channel->Clips[j]);
//				if (clip_within_box( cclip, i_StartX, i_StartY, i_CurrentX, i_CurrentY ))
//				{
//					//	select this clip
//					if (!IsClipSelected(cclip))
//					{
//						channel->BeginInteraction();
//
//						cclip->Select();
//						const bool bAppend = true;
//						channel->SelectClip(j,bAppend);
//
//						channel->FinishInteraction();
//						channel->Invalidate();
//					}
//				}
//			}
//		}
//	}
//}
#endif // USE_WXWIDGETS
