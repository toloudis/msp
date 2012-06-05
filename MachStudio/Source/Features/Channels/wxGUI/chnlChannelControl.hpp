/****************************************************************************\
**	chnlChannelControl.hpp
**
**		Custom control for displaying clips on a channel
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_CHANNELCONTROL_HPP
#error chnlChannelControl.hpp multiply included
#endif
#define CHNL_CHANNELCONTROL_HPP

#ifndef CHNL_TIMECOMMON_HPP
#include "Features/Channels/wxGUI/chnlTimeCommon.hpp"
#endif 
#ifndef CHNL_CHANNELCLIP_HPP
#include "Features/Channels/wxGUI/chnlChannelClip.hpp"
#endif

#include <vector>
#include <set>

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class chnlChannelClip;

//============================================================================
//============================================================================
class chnlChannelControl : public wxControl,
							public chnlTimeCommon,
							public chnlChannelClip::ChangedCallback
{
	public:
		//--------------------------------------------------------------------
		// Callback type for being notified when an interaction has happened
		//--------------------------------------------------------------------
		class InteractionCallback
		{
		public:
			virtual void ClipSelected(chnlChannelControl*, chnlChannelClip*, bool i_bAppended) = 0;
			virtual void ClipDeselected(chnlChannelControl*, chnlChannelClip*) = 0;
			virtual void ClipMoved(chnlChannelControl*, float i_TimeDelta) = 0;
			virtual void ClipMoveFinished(chnlChannelControl*) = 0;
			virtual void ClipResized(chnlChannelControl*) = 0;
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlChannelControl(wxWindow* i_pParent, bool i_bLocked);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~chnlChannelControl();

		//--------------------------------------------------------------------
		// Set callback pointer for when an interaction has happened
		//--------------------------------------------------------------------
		void SetInteractionCallback(InteractionCallback* i_pCallback);

		//--------------------------------------------------------------------
		// Locked - prevents user from altering clips within this channel
		//--------------------------------------------------------------------
		inline bool GetLocked() const;
		void SetLocked(bool i_bLocked);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Add Clip to list - this control takes ownership of the clip
		//--------------------------------------------------------------------
		void AddClip(chnlChannelClip* i_pClip);

		//--------------------------------------------------------------------
		// Selection functions
		//--------------------------------------------------------------------
		void ClearSelection();
		void DeselectClip(chnlChannelClip* i_pClip);
		void SelectClip(chnlChannelClip* i_pClip, bool i_bAppend = false);
        void SelectClips(float i_StartTime, float i_EndTime);

		//--------------------------------------------------------------------
		// If this channel has a selected clip, select all drivers
		//	after this selected clip.
		//--------------------------------------------------------------------
		void SelectClipsAfter();

		//--------------------------------------------------------------------
		// Access to clips
		//--------------------------------------------------------------------
		int GetNumClips() const;
		chnlChannelClip* GetClip(int i_Index);
		const std::set<chnlChannelClip*>& GetSelectedClips() const;
				
		//--------------------------------------------------------------------
		// Sets the InteractionStart and InteractionDuration values 
		// of the selected clips from the current BeginTime and Duration. 
		// This is related to the ClipSelected callback.
		//--------------------------------------------------------------------
		void BeginInteraction();

		//--------------------------------------------------------------------
		// Offsets the InteractStart of all selected clips from 
		// BeginTime by the given TimeDelta. This is related to the
		// ClipMoved callback.
		//--------------------------------------------------------------------
		void InteractMoveSelectedClips(float i_TimeDelta);

		//--------------------------------------------------------------------
		// Sets the InteractionStart and InteractionDuration values for
		// each selected clip into the permanent BeginTime and Duration
		// properties. This is related to the ClipMoveFinished callback.
		//--------------------------------------------------------------------
		void FinishInteraction();

		//--------------------------------------------------------------------
		// SetSnapInterval - set unit to snap to as number of parts 
		//	per second. To snap to 30fps, set the snap interval to 30.
		//	Default is 120.
		//--------------------------------------------------------------------
		void SetSnapInterval(float i_Interval);

	private:
		//--------------------------------------------------------------------
		// Virtual function called when time range or scale has changed
		//--------------------------------------------------------------------
		virtual void update_size();

		//----------------------------------------------------------------------------
		// private functions
		//----------------------------------------------------------------------------
		void sort_clips();
		void compute_size();
		void get_rectangle(const chnlChannelClip& i_Clip, 
						   int &o_X, int &o_Y, int &o_Width, int &o_Height);
		void set_tip_description();
		chnlChannelClip* pick_clip(int i_eX, int i_eY, bool &o_Edge);
		
		//--------------------------------------------------------------------
		// Called when a clip of ours changes
		//--------------------------------------------------------------------
		virtual void ClipChanged(chnlChannelClip*);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void OnPaint(wxPaintEvent &i_Event);
		void OnMouseDown(wxMouseEvent &i_Event);
		void OnMouseMove(wxMouseEvent &i_Event);
		void OnCaptureLost(wxMouseCaptureLostEvent &i_Event);
		void OnMouseUp(wxMouseEvent &i_Event);
		void OnSize(wxSizeEvent &i_Event);
		void OnContextMenu(wxContextMenuEvent& i_Event);
		void OnDoubleClick(wxMouseEvent& i_Event);
		void OnShowProperties(wxCommandEvent& i_Event);
		void OnSelectIcon(wxCommandEvent& i_Event);
		void OnSetBlendNone(wxCommandEvent& i_Event);
		void OnSetBlendPrevious(wxCommandEvent& i_Event);
		void OnSetBlendSmooth(wxCommandEvent& i_Event);
		void OnCopy(wxCommandEvent& i_Event);
		void OnCut(wxCommandEvent& i_Event);
		void OnPaste(wxCommandEvent& i_Event);
		std::vector<chnlChannelClip*> m_Clips;
		chnlChannelClip* m_pSelectedClip;
		std::set<chnlChannelClip*> m_SelectedClips;
		bool m_bLocked;
		// Snap to time based on this interval per second
		int m_SnapInterval;

		// Mouse Interaction state
		enum InteractionType
		{
			e_None = 0,		// no interaction in this channel
			e_Moving,		// moving start time of clip without changing duration
			e_Resizing		// changing duration of clip
		};
		enum ResizingType
		{
			e_Right = 0,
			e_Left
		};

		InteractionType m_Interaction;
		ResizingType m_ResizingType;
		int m_IX;
		int m_CurX, m_CurY;
		// have to move a few pixels before really changing the driver.
		// This is set true after the interaction has involved a substantial
		// mouse movement (more than a few pixels as defined below.
		bool m_bInteractionConfirmed;	
		InteractionCallback* m_pInteractionCallback;

    DECLARE_EVENT_TABLE()
};

//--------------------------------------------------------------------
// Locked - prevents user from altering clips within this channel
//--------------------------------------------------------------------
inline bool chnlChannelControl::GetLocked() const
{
	return m_bLocked;
}

#endif // USE_WXWIDGETS
