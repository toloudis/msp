/****************************************************************************\
**	chnlTraxDialog.hpp
**
**		wxWidgets dialog for editing drivers in channels
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TRAXDIALOG_HPP
#error chnlTraxDialog.hpp multiply included
#endif
#define CHNL_TRAXDIALOG_HPP

#ifndef CHNL_CHANNELCONTROL_HPP
#include "Features/Channels/wxGUI/chnlChannelControl.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif 


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
class chnlMarkerBar;
class chnlTimeLabel;
class chnlTimeSlider;
class maTime;
class prtyProperty;
class tmlnChannel;
class tmlnDriver;
class tmlnScriptObject;
class twcRangedFloat;

//============================================================================
/// Class chnlTraxDialog
//============================================================================
class chnlTraxDialog : public wxPanel, //wxDialog ,
						public chnlChannelControl::InteractionCallback
{
	public:
		//--------------------------------------------------------------------
		///	Static pointer to the instance of the form, will be cleared
		///	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static chnlTraxDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTraxDialog( wxWindow* parent, 
					   wxWindowID id = wxID_ANY, 
					   const wxString& title = wxT("Timeline Editor"), 
					   const wxPoint& pos = wxDefaultPosition, 
					   const wxSize& size = wxSize( 800, 300 ), 
					   long style = 0); //wxCAPTION|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU );
		
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chnlTraxDialog();

		//--------------------------------------------------------------------
		/// Update channels based on selection
		//--------------------------------------------------------------------
		void UpdateChannels();
		void Select(tmlnScriptObject* i_pObject);

		//--------------------------------------------------------------------
		/// Remove all channel  controls from form
		//--------------------------------------------------------------------
		void ClearChannels();
					
		//--------------------------------------------------------------------
		/// SetTotalTime changes length of timeline controls
		//--------------------------------------------------------------------
		///void SetTotalTime(float i_Time);
		//--------------------------------------------------------------------
		/// SetTimeRange changes length of timeline controls, setting
		///	 minimum and maximum time range.
		//--------------------------------------------------------------------
		void SetTimeRange(const maTime& i_MinTime, const maTime& i_MaxTime);

		//----------------------------------------------------------------------------
		/// Zoom time scale so that full timeline is visible
		//----------------------------------------------------------------------------
		void CalculateMaxZoom();

		void UpdateScale();

		//--------------------------------------------------------------------
		/// TimeScale changes the zoom left/right
		//--------------------------------------------------------------------
		void SetTimeScale(float i_Scale);

		//--------------------------------------------------------------------
		/// SetSnapInterval alters grid unit for moving drivers in channels
		//--------------------------------------------------------------------
		void SetSnapInterval(int i_Interval);
				
		//--------------------------------------------------------------------
		///  This driver has changed its properties related to the
		///	trax editor display, so update the channels related to it.
		//--------------------------------------------------------------------
		void UpdateDriver(tmlnDriver *i_pDriver);

		//--------------------------------------------------------------------
		/// Time format has changed, alter display
		//--------------------------------------------------------------------
		void UpdateTimeLabel();

		//--------------------------------------------------------------------
		/// Timeline time has changed, alter display
		//--------------------------------------------------------------------
		void UpdateCurrentTime();

		//--------------------------------------------------------------------
		/// Look for time ticks based on begin and end times of drivers
		//--------------------------------------------------------------------
		void UpdateTimeTicks();

		//--------------------------------------------------------------------
		/// Update marker bar's display of markers and notes
		//--------------------------------------------------------------------
		void UpdateMarkersAndNotes();
		
		//------------------------------------------------------------------------
		/// Get set of drivers that are selected in the trax editor. If skip locked
		/// is true, then only drivers in channels that are unlocked will be returned.
		//------------------------------------------------------------------------
		void GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers, 
												bool i_bSkipLocked);

		//------------------------------------------------------------------------
		/// Deselect all drivers in the channel interface
		//------------------------------------------------------------------------
		void ClearSelection();

		//------------------------------------------------------------------------
		/// Select driver in the channel interface - this will do an append
		///	to the selection, it will not deselect other drivers.
		//------------------------------------------------------------------------
		void AddToSelection( tmlnDriver* i_pDriver );

		//------------------------------------------------------------------------
		/// Get and Set Zoom slider scale
		//------------------------------------------------------------------------
		float GetSliderZoom();
		void SetSliderZoom(float zoom);

		

	private:
		//--------------------------------------------------------------------
		/// Interaction Callbacks from channel controls
		//--------------------------------------------------------------------
		virtual void ClipSelected(chnlChannelControl*, chnlChannelClip*, bool i_bAppended);
		virtual void ClipDeselected(chnlChannelControl*, chnlChannelClip*);
		virtual void ClipMoved(chnlChannelControl*, float i_TimeDelta);
		virtual void ClipMoveFinished(chnlChannelControl*);
		virtual void ClipResized(chnlChannelControl*);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent& i_Event);
		void ChannelPanel_MouseDown( wxMouseEvent& i_Event );
		void ChannelPanel_MouseUp( wxMouseEvent& i_Event );
		void ChannelPanel_MouseMove( wxMouseEvent& i_Event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void TimeChanged(wxCommandEvent &i_Event);
		void ZoomChanged(wxCommandEvent &i_Event);
		void FilterIndexChanged(wxCommandEvent& i_Event);
		void checkBox_Snap_Clicked(wxCommandEvent& i_Event);

		//--------------------------------------------------------------------
		/// button events
		//--------------------------------------------------------------------
		void button_ZoomCenter_Click( wxCommandEvent& i_Event );
		void button_PrevTick_Click( wxCommandEvent& i_Event );
		void button_NextTick_Click( wxCommandEvent& i_Event );
		void button_Add_Click( wxCommandEvent& i_Event );
		void button_Delete_Click( wxCommandEvent& i_Event );
		void button_Copy_Click( wxCommandEvent& i_Event );
		void button_Paste_Click( wxCommandEvent& i_Event );
		void button_Split_Click( wxCommandEvent& i_Event );
		void button_Move_Click( wxCommandEvent& i_Event );
		void MoveAmountChanged(prtyProperty *i_pProperty, bool i_bDirty);
		void button_Sel_Click( wxCommandEvent& i_Event );
		void button_AddMarker_Click( wxCommandEvent& i_Event );
		void button_DeleteMarker_Click( wxCommandEvent& i_Event );
		void button_PrevMarker_Click( wxCommandEvent& i_Event );
		void button_NextMarker_Click( wxCommandEvent& i_Event );
		void button_AddNote_Click( wxCommandEvent& i_Event );
		void button_DeleteNote_Click( wxCommandEvent& i_Event );
		void button_PrevNote_Click( wxCommandEvent& i_Event );
		void button_NextNote_Click( wxCommandEvent& i_Event );

	private:
		//--------------------------------------------------------------------
		/// private functions
		//--------------------------------------------------------------------
		void multiple_selected_display();
		void setup_display(tmlnScriptObject* i_pObject);
		void single_object_display(tmlnScriptObject* i_pObject);
		void system_object_display(tmlnScriptObject* i_pObject);
		void all_object_display();
		void add_channels_for_object(tmlnScriptObject* i_pObject);
		void add_category(const itString& i_CategoryName);
		chnlChannelControl* add_channel(tmlnChannel& i_Channel);
		void resize_scrolled_windows();
		void current_time_changed();
		bool any_drivers_selected();
		void center_view();
		void center_view_on_currenttime();
		float get_time_from_point(int i_CurrentX, int i_CurrentY);
		void select_clips_within_time( float i_StartTime, float i_CurrentTime, bool shiftIsPressed );
		//----------------------------------------------------------------------------
		//Driver Conflict functions
		//----------------------------------------------------------------------------
		void show_driver_conflict_error(int i_NumConflicts);
		bool check_driver_conflicts(chnlChannelClip* i_Clip1, const maTime& i_Delta, chnlChannelClip* i_Clip2,chnlChannelControl* i_channel) const; 
		int check_channel_for_driver_conflicts(chnlChannelControl* i_pchannel, chnlChannelClip* i_pClip, const maTime& i_Delta) const;
		int check_channel_for_driver_conflicts(chnlChannelControl* i_pchannel, const maTime& i_Delta) const;
		int check_channel_for_driver_conflicts() const;

		//bool clip_within_box( ChannelClip^ i_Clip, int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY );
		//void select_clips_within_box( int i_StartX, int i_StartY, int i_CurrentX, int i_CurrentY );
		
		int m_InitX, m_InitY;
		bool m_bleftIsDown, m_bIsDragging;
		wxPoint endPoint, savePoint;
		int width, height;

		std::vector<chnlChannelControl*> m_Channels;
		int m_nFilterState;
		int m_SnapInterval;

		///	for marquee-style select
		bool m_bDriverSelectStart;
		int m_DriverSelectStartX;
		int m_DriverSelectStartY;
		float m_DriverSelectStartTime;

		/// Controls
		twcRangedFloat* m_slider_zoom;
		wxButton* m_button_ZoomCenter;
		wxChoice* m_choice_Display;
		wxCheckBox* m_checkBox_Snap;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
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
		wxButton* m_button_PrevTick;
		wxButton* m_button_NextTick;
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
		wxSize windowWidth;
		int windowW, windowH;
		void PaintRectangle(wxDC &i_DC, int i_mCurX, int i_mCurY);
	
};

#endif // USE_WXWIDGETS
