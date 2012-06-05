/****************************************************************************\
**	chnlTraxFrame.hpp
**
**		Data class for a clip to display in a channel control
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TRAXFRAME_HPP
#error chnlTraxFrame.hpp multiply included
#endif
#define CHNL_TRAXFRAME_HPP

#ifndef CHNL_CHANNELCONTROL_HPP
#include "Features/Channels/wxGUI/chnlChannelControl.hpp"
#endif
#ifndef CHNL_MARKERICON_HPP
#include "Features/Channels/wxGUI/chnlMarkerIcon.hpp"
#endif 
#ifndef CHNL_NOTEICON_HPP
#include "Features/Channels/wxGUI/chnlNoteIcon.hpp"
#endif 

#include <vector>

#ifdef USE_WXWIDGETS

#include <wx/slider.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/choice.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/scrolwin.h>
#include <wx/dialog.h>

//============================================================================
//============================================================================
class chnlChannelControl;
class chnlTimeLabel;
class chnlTimeSlider;
class chnlMarkerBar;
class twcRangedFloat;

//============================================================================
/// Class chnlTraxFrame
//============================================================================
class chnlTraxFrame : public wxFrame ,
						public chnlChannelControl::InteractionCallback
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTraxFrame( wxWindow* parent, 
					   wxWindowID id = wxID_ANY, 
					   const wxString& title = wxT("Channel Editor"), 
					   const wxPoint& pos = wxDefaultPosition, 
					   const wxSize& size = wxSize( 548,461 ), 
					   long style = wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU );
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chnlTraxFrame();

		//--------------------------------------------------------------------
		// Remove all channel  controls from form
		//--------------------------------------------------------------------
		void ClearChannels();

		//--------------------------------------------------------------------
		// Add category label to form
		//--------------------------------------------------------------------
		void AddCategory(const std::string& i_CategoryName);

		//--------------------------------------------------------------------
		// Add channel control and label to form
		//--------------------------------------------------------------------
		chnlChannelControl* AddChannel(const std::string& i_ChannelName);
					
		//--------------------------------------------------------------------
		// SetTotalTime changes length of timeline controls
		//--------------------------------------------------------------------
		void SetTotalTime(float i_Time);

		//--------------------------------------------------------------------
		// TimeScale changes the zoom left/right
		//--------------------------------------------------------------------
		void SetTimeScale(float i_Scale);

		//--------------------------------------------------------------------
		// AddTimeTick adds tick to time slider
		//--------------------------------------------------------------------
		void AddTimeTick(float i_Time);

		//--------------------------------------------------------------------
		// AddMarker adds marker to marker bar
		//--------------------------------------------------------------------
		void AddMarker(float i_Time, 
						chnlMarkerIcon::MarkerType i_Type, 
						const std::string& i_Note);

		//--------------------------------------------------------------------
		// AddNote adds note to marker bar
		//--------------------------------------------------------------------
		void AddNote(float i_Time, 
						chnlNoteIcon::NoteStatus i_Status, 
						const std::string& i_Note);

	private:
		//--------------------------------------------------------------------
		// Interaction Callbacks from channel controls
		//--------------------------------------------------------------------
		virtual void ClipSelected(chnlChannelControl*, chnlChannelClip*, bool i_bAppended);
		virtual void ClipMoved(chnlChannelControl*, float i_TimeDelta);
		virtual void ClipMoveFinished(chnlChannelControl*);
		virtual void ClipResized(chnlChannelControl*);

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		void resize_scrolled_windows();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void TimeChanged(wxCommandEvent &i_Event);
		void ZoomChanged(wxCommandEvent &i_Event);

		std::vector<chnlChannelControl*> m_Channels;

		// Controls
		twcRangedFloat* m_slider_zoom;
		wxButton* m_button_zoomCenter;
		wxChoice* m_choice_Display;
		wxCheckBox* m_checkBox_Snap;
		wxStaticText* m_staticText1;
		wxButton* m_button_RemoveNote;
		wxButton* m_button_AddNote;
		wxButton* m_button_PrevNote;
		wxButton* m_button_NextNote;
		wxStaticText* m_staticText11;
		wxButton* m_button_RemoveMarker;
		wxButton* m_button_AddMarker;
		wxButton* m_button_PrevMarker;
		wxButton* m_button_NextMarker;
		wxScrolledWindow* m_scrollwin_timeline;
		wxBoxSizer* m_bSizer_timeline;
		chnlTimeLabel* m_timeLabel;
		chnlTimeSlider* m_timeSlider;
		chnlMarkerBar* m_markerBar;
		wxStaticLine* m_staticline_markers;
		wxButton* m_button17;
		wxButton* m_button18;
		wxButton* m_button_Add;
		wxButton* m_button_Delete;
		wxButton* m_button_Copy;
		wxButton* m_button_Paste;
		wxButton* m_button_Split;
		wxButton* m_button_Move;
		wxButton* m_button_Sel;
		wxScrolledWindow* m_scrollwin_names;
		wxBoxSizer* m_bSizer_names;
		//wxStaticText* m_staticText_category;
		//wxStaticText* m_staticText_channelName;
		//wxCheckBox* m_checkBox_locked;
		wxScrolledWindow* m_scrollwin_channels;
		wxBoxSizer* m_bSizer_channels;
	
};

#endif // USE_WXWIDGETS
