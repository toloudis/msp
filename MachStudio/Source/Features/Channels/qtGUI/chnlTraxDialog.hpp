/****************************************************************************\
**	chnlTraxDialog.hpp
**
**		Qt pane for editing drivers in channels
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TRAXDIALOG_HPP
#error chnlTraxDialog.hpp multiply included
#endif
#define CHNL_TRAXDIALOG_HPP

#ifndef CHNL_CHANNELCONTROL_HPP
#include "Features/Channels/qtGUI/chnlChannelControl.hpp"
#endif
#include <string>

#ifdef USE_QT

class chnlChannelControl;
class chnlMarkerBar;
class chnlTimeLabel;
class chnlTimeSlider;
class maTime;
class prtyProperty;
class tmlnChannel;
class tmlnDriver;
class tmlnScriptObject;
class tqcRangedFloat;
class QCheckBox;
class QComboBox;
class QLabel;
class QScrollArea;
class QVBoxLayout;

//============================================================================
//============================================================================
class chnlTraxDialog : public QWidget,
						public chnlChannelControl::InteractionCallback
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static chnlTraxDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTraxDialog( QWidget* parent,
					   const char* title = "Timeline Editor" );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chnlTraxDialog();

		//--------------------------------------------------------------------
		// Update channels based on selection
		//--------------------------------------------------------------------
		void UpdateChannels();
		void Select(tmlnScriptObject* i_pObject);

		//--------------------------------------------------------------------
		// Remove all channel  controls from form
		//--------------------------------------------------------------------
		void ClearChannels();

		//--------------------------------------------------------------------
		// SetTimeRange changes length of timeline controls, setting
		//	 minimum and maximum time range.
		//--------------------------------------------------------------------
		void SetTimeRange(const maTime& i_MinTime, const maTime& i_MaxTime);

		//--------------------------------------------------------------------
		// Zoom time scale so that full timeline is visible
		//--------------------------------------------------------------------
		void CalculateMaxZoom();

		//--------------------------------------------------------------------
		//	Reset the scale.  This is important when the user changes the
		//	global frame rate and the channel drivers need to resize
		//--------------------------------------------------------------------
		void UpdateScale();

		//--------------------------------------------------------------------
		// TimeScale changes the zoom left/right
		//--------------------------------------------------------------------
		void SetTimeScale(float i_Scale);

		//--------------------------------------------------------------------
		// SetSnapInterval alters grid unit for moving drivers in channels
		//--------------------------------------------------------------------
		void SetSnapInterval(int i_Interval);

		//--------------------------------------------------------------------
		//  This driver has changed its properties related to the
		//	trax editor display, so update the channels related to it.
		//--------------------------------------------------------------------
		void UpdateDriver(tmlnDriver *i_pDriver);

		//--------------------------------------------------------------------
		// Time format has changed, alter display
		//--------------------------------------------------------------------
		void UpdateTimeLabel();

		//--------------------------------------------------------------------
		// Timeline time has changed, alter display
		//--------------------------------------------------------------------
		void UpdateCurrentTime();

		//--------------------------------------------------------------------
		// Look for time ticks based on begin and end times of drivers
		//--------------------------------------------------------------------
		void UpdateTimeTicks();

		//--------------------------------------------------------------------
		// Update marker bar's display of markers and notes
		//--------------------------------------------------------------------
		void UpdateMarkersAndNotes();

		//--------------------------------------------------------------------
		// Get set of drivers that are selected in the trax editor. If skip
		// locked is true, then only drivers in channels that are unlocked
		// will be returned.
		//--------------------------------------------------------------------
		void GetSelectedDrivers(std::set<tmlnDriver*> &o_Drivers,
								bool i_bSkipLocked);

		//--------------------------------------------------------------------
		// Deselect all drivers in the channel interface
		//--------------------------------------------------------------------
		void ClearSelection();

		//--------------------------------------------------------------------
		// Select driver in the channel interface - this will do an append
		//	to the selection, it will not deselect other drivers.
		//--------------------------------------------------------------------
		void AddToSelection( tmlnDriver* i_pDriver );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		float GetSliderZoom();
		void SetSliderZoom(float zoom);

	private:
		//--------------------------------------------------------------------
		// Interaction Callbacks from channel controls
		//--------------------------------------------------------------------
		virtual void ClipSelected(chnlChannelControl*, chnlChannelClip*, bool i_bAppended);
		virtual void ClipDeselected(chnlChannelControl*, chnlChannelClip*);
		virtual void ClipMoved(chnlChannelControl*, float i_TimeDelta);
		virtual void ClipMoveFinished(chnlChannelControl*);
		virtual void ClipResized(chnlChannelControl*);

	public:
		//--------------------------------------------------------------------
		// Mouse events on the channel panel behind the channels select
		//	the drivers within the dragged time span.
		//--------------------------------------------------------------------
		void ChannelPanel_MouseDown( QMouseEvent* i_pEvent );
		void ChannelPanel_MouseUp( QMouseEvent* i_pEvent );
		void ChannelPanel_MouseMove( QMouseEvent* i_pEvent );

	protected:
		//--------------------------------------------------------------------
		// Fit the time scale to the new width, unless the user has zoomed
		//--------------------------------------------------------------------
		virtual void resizeEvent(QResizeEvent* i_pEvent);

	private:
		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void FilterIndexChanged();
		void checkBox_Snap_Clicked();
		void button_Move_Click();
		void MoveAmountChanged(prtyProperty *i_pProperty, bool i_bDirty);

		void multiple_selected_display();
		void setup_display(tmlnScriptObject* i_pObject);
		void single_object_display(tmlnScriptObject* i_pObject);
		void system_object_display(tmlnScriptObject* i_pObject);
		void all_object_display();
		void add_channels_for_object(tmlnScriptObject* i_pObject);
		void add_category(const std::string& i_CategoryName);
		chnlChannelControl* add_channel(tmlnChannel& i_Channel);
		void resize_scrolled_windows();
		void current_time_changed();
		bool any_drivers_selected();
		void center_view();
		void center_view_on_currenttime();
		float get_time_from_point(int i_CurrentX, int i_CurrentY);
		void select_clips_within_time( float i_StartTime, float i_CurrentTime, bool shiftIsPressed );
		void show_driver_conflict_error(int i_NumConflicts);
		bool check_driver_conflicts(chnlChannelClip* i_Clip1, const maTime& i_Delta, chnlChannelClip* i_Clip2,chnlChannelControl* i_channel) const;
		int check_channel_for_driver_conflicts(chnlChannelControl* i_pchannel, chnlChannelClip* i_pClip, const maTime& i_Delta) const;
		int check_channel_for_driver_conflicts(chnlChannelControl* i_pchannel, const maTime& i_Delta) const;
		int check_channel_for_driver_conflicts() const;

		std::vector<chnlChannelControl*> m_Channels;
		int m_nFilterState;
		int m_SnapInterval;

		bool m_bDriverSelectStart;
		float m_DriverSelectStartTime;

		// The time scale follows the width of the panel (as CalculateMaxZoom
		// sets it) until the user zooms. The panel only gets its width once
		// the main window is laid out, after the time range was first set.
		bool m_bFitZoomToWidth;
		bool m_bFittingZoom;

		tqcRangedFloat* m_slider_zoom;
		QComboBox* m_choice_Display;
		QCheckBox* m_checkBox_Snap;
		QLabel* m_staticText2;

		QScrollArea* m_scrollwin_timeline;
		chnlTimeLabel* m_timeLabel;
		chnlTimeSlider* m_timeSlider;
		chnlMarkerBar* m_markerBar;

		QScrollArea* m_scrollwin_names;
		QVBoxLayout* m_bSizer_names;
		QScrollArea* m_scrollwin_channels;
		QWidget* m_pChannelPanel;
		QVBoxLayout* m_bSizer_channels;
};

#endif // USE_QT
