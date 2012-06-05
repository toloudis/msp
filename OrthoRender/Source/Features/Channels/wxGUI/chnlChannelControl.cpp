/****************************************************************************\
**	chnlChannelControl.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/wxGUI/chnlChannelControl.hpp"
#include "Features/Channels/wxGUI/chnlTimelineGuideUtil.hpp"

#include "Features/Channels/chnlOperations.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/ma/maFunctions.hpp"

#include <algorithm>

#ifdef USE_WXWIDGETS

namespace
{
	const int c_IconCenter = 12;
	const int c_ChannelHeight = 28; // 24 height + 4 buffer on each side
	// To display "keys" better, short duration drivers are represented differently.
	const float c_ShortKeyDuration = (1.0f / 15.0f); // just one frame, or short period?
	// If clips are less than this length, turn off the mouse resize interaction
	const float c_MinimumInteractionDuration = 0.1f; 
	// Short drivers have circle added, this defines radius in pixels of this circle
	const int c_KeyCircleRadius = 4; 
	// amount to move mouse before interaction is confirmed
	const int c_PixelMoveThreshold = 2; 

	//--------------------------------------------------------------------
	// IDs for the menu commands
	//--------------------------------------------------------------------
	enum
	{
		MENU_SHOW_PROPERTIES = wxID_HIGHEST,
		MENU_SELECTICON,
		MENU_CUT,
		MENU_COPY,
		MENU_PASTE
	};

	bool clip_compare(chnlChannelClip* i_pClip1, chnlChannelClip* i_pClip2)
	{
		return (i_pClip1->GetBeginTime() < i_pClip2->GetBeginTime());
	}
	
	void clear_selected(std::set<chnlChannelClip*> &io_Clips)
	{
        //foreach (int i in io_Clips)
        //{
        //    this.Clips[i].UseHighlightFill = false;
        //}
        io_Clips.clear();
	}

	// snap time to an interval based on number passed in
	float snap_time(float i_Time, int i_Interval)
	{
		float up = i_Time * i_Interval;
		int rounded = (int) up;
		return  (rounded / (float) i_Interval);
	}
}


//--------------------------------------------------------------------
// event table
//--------------------------------------------------------------------
BEGIN_EVENT_TABLE(chnlChannelControl, wxControl)
    EVT_PAINT(chnlChannelControl::OnPaint)
	EVT_RIGHT_DOWN(chnlChannelControl::OnMouseDown)
	EVT_LEFT_DOWN(chnlChannelControl::OnMouseDown)
	EVT_LEFT_UP(chnlChannelControl::OnMouseUp)
    EVT_MOTION(chnlChannelControl::OnMouseMove)
	EVT_SIZE(chnlChannelControl::OnSize)
    EVT_CONTEXT_MENU(chnlChannelControl::OnContextMenu)
	EVT_LEFT_DCLICK(chnlChannelControl::OnDoubleClick)
    EVT_MENU(MENU_SHOW_PROPERTIES, chnlChannelControl::OnShowProperties)
    EVT_MENU(MENU_SELECTICON, chnlChannelControl::OnSelectIcon)
	EVT_MENU(MENU_CUT, chnlChannelControl::OnCut)
	EVT_MENU(MENU_COPY, chnlChannelControl::OnCopy)
	EVT_MENU(MENU_PASTE, chnlChannelControl::OnPaste)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlChannelControl::chnlChannelControl(wxWindow* i_pParent, bool i_bLocked)
:	wxControl(i_pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER),
	m_pSelectedClip(NULL),
	m_bLocked(i_bLocked),
	m_SnapInterval(120),
	m_Interaction(e_None),
	m_ResizingType(e_Right),
	m_IX(0),
	m_CurX(0),
	m_CurY(0),
	m_bInteractionConfirmed(false),
	m_pInteractionCallback(NULL)
{	
	this->compute_size();	
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chnlChannelControl::~chnlChannelControl()
{
	envSTLHelpers::DeleteContainer(m_Clips);
}

//--------------------------------------------------------------------
// Set callback pointer for when an interaction has happened
//--------------------------------------------------------------------
void chnlChannelControl::SetInteractionCallback(InteractionCallback* i_pCallback)
{
	m_pInteractionCallback = i_pCallback;
}

//--------------------------------------------------------------------
// Locked - prevents user from altering clips within this channel
//--------------------------------------------------------------------
void chnlChannelControl::SetLocked(bool i_bLocked)
{
	m_bLocked = i_bLocked;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void chnlChannelControl::Clear()
{
	m_pSelectedClip = NULL;
	m_SelectedClips.clear();
	envSTLHelpers::DeleteContainer(m_Clips);

	this->compute_size();
	this->Refresh();
}

//--------------------------------------------------------------------
/// Add Clip to list - this control takes ownership of the clip
//--------------------------------------------------------------------
void chnlChannelControl::AddClip(chnlChannelClip* i_pClip)
{
	m_Clips.push_back(i_pClip);

	// register callback for when clip changes
	i_pClip->SetChangedCallback( this );

	this->sort_clips();
	this->compute_size();
	this->Refresh();
}

//--------------------------------------------------------------------
// Selection functions
//--------------------------------------------------------------------
void chnlChannelControl::ClearSelection()
{
	this->SelectClip(NULL);
}
void chnlChannelControl::SelectClip(chnlChannelClip* i_pClip, bool i_bAppend)
{
	m_pSelectedClip = i_pClip; 
	if (!i_bAppend)
	{
		clear_selected(this->m_SelectedClips);
	}
	if (i_pClip)
	{
		this->m_SelectedClips.insert(i_pClip);
	}
	this->Refresh();
		
	// Call virtual "Select" function to allow driver to 
	//  handle this event
	if (m_pSelectedClip)
	{
		m_pSelectedClip->Select();
	}
}

void chnlChannelControl::SelectClips(float i_StartTime, float i_EndTime)
{
	const int num_clips = m_Clips.size();
	for (int ci=0; ci<num_clips; ++ci)
	{
		chnlChannelClip *pClip = m_Clips[ci];

		int chnl_root = 0; // get_channel_root(clip);

        if (((pClip->GetBeginTime() >= i_StartTime) && (pClip->GetBeginTime() <= i_EndTime))
            //|| ((pClip->GetEndTime() >= i_StartTime) && (pClip->GetEndTime() <= i_EndTime))
            )
        {
            SelectClip(pClip, true);    // append
        }
	}
}

//--------------------------------------------------------------------
// If this channel has a selected clip, select all drivers
//	after this selected clip.
//--------------------------------------------------------------------
void chnlChannelControl::SelectClipsAfter()
{
	if (!m_bLocked && !m_SelectedClips.empty())
	{
		// Get earliest selected clip
		std::set<chnlChannelClip*>::iterator min_it = 
			std::min_element(m_SelectedClips.begin(), m_SelectedClips.end(), clip_compare);
		if (min_it != m_SelectedClips.end())
		{
			float start_time = (*min_it)->GetBeginTime();
			const int num_clips = m_Clips.size();
			for (int ci=0; ci<num_clips; ++ci)
			{
				chnlChannelClip *pClip = m_Clips[ci];
				if (pClip->GetBeginTime() >= start_time)
				{
					SelectClip(pClip, true);    // append
				}
			}

		}
	}
}

//--------------------------------------------------------------------
// Access to clips
//--------------------------------------------------------------------
int chnlChannelControl::GetNumClips() const
{
	return m_Clips.size();
}
chnlChannelClip* chnlChannelControl::GetClip(int i_Index)
{
	DBG_ASSERT2(i_Index>=0 && i_Index<m_Clips.size(), "GetClip %d out of range %d", i_Index, m_Clips.size());
	return m_Clips[i_Index];
}
const std::set<chnlChannelClip*>& chnlChannelControl::GetSelectedClips() const
{
	return m_SelectedClips;
}

//--------------------------------------------------------------------
// Sets the InteractionStart and InteractionDuration values 
// of the selected clips from the current BeginTime and Duration. 
// This is related to the ClipSelected callback.
//--------------------------------------------------------------------
void chnlChannelControl::BeginInteraction()
{
	std::set<chnlChannelClip*>::iterator it;
	for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
	{
		chnlChannelClip *sel_clip = (*it);
		sel_clip->SetInteraction( sel_clip->GetBeginTime(), sel_clip->GetDuration());
		//sel_clip.InteractRoot = get_channel_root(sel_clip);
	}
}

//--------------------------------------------------------------------
// Offsets the InteractStart of all selected clips from 
// BeginTime by the given TimeDelta. This is related to the
// ClipMoved callback.
//--------------------------------------------------------------------
void chnlChannelControl::InteractMoveSelectedClips(float i_TimeDelta)
{
	std::set<chnlChannelClip*>::iterator it;
	for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
	{
		chnlChannelClip *sel_clip = (*it);

		float interact_start = sel_clip->GetBeginTime() + i_TimeDelta;

		// I am not sure what to enforce as the minimum time anymore. 
		// Maybe we can allow the begin time to go below the minimum as long as
		// the end time doesn't?
		float min_time = maFunctions::Lowest(tmlnTimeLine::GetMinimum(), sel_clip->GetBeginTime());
		if (interact_start < min_time) interact_start = min_time;

		interact_start = snap_time(interact_start, m_SnapInterval);
		sel_clip->SetInteraction( interact_start, sel_clip->GetInteractDuration());
	}
	if (!m_SelectedClips.empty())
		this->Refresh();
}


//--------------------------------------------------------------------
// Sets the InteractionStart and InteractionDuration values for
// each selected clip into the permanent BeginTime and Duration
// properties. This is related to the ClipMoveFinished callback.
//--------------------------------------------------------------------
void chnlChannelControl::FinishInteraction()
{
	std::set<chnlChannelClip*>::iterator it;
	for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
	{
		chnlChannelClip *sel_clip = (*it);

		if ( (sel_clip->GetBeginTime() != sel_clip->GetInteractStart())
			|| (sel_clip->GetDuration() != sel_clip->GetInteractDuration()) )
		{
			sel_clip->SetTime(sel_clip->GetInteractStart(), sel_clip->GetInteractDuration());
		}
	}
	if (!m_SelectedClips.empty())
	{
		this->sort_clips();
		this->Refresh();
	}
}

//--------------------------------------------------------------------
// SetSnapInterval - set unit to snap to as number of parts 
//	per second. To snap to 30fps, set the snap interval to 30.
//	Default is 120.
//--------------------------------------------------------------------
void chnlChannelControl::SetSnapInterval(float i_Interval)
{
	if (i_Interval > 0)
		m_SnapInterval = i_Interval;
}

//--------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//--------------------------------------------------------------------
void chnlChannelControl::update_size()
{
	this->compute_size();
	this->Refresh();
}

//----------------------------------------------------------------------------
// sort clips by begin time
//----------------------------------------------------------------------------
void chnlChannelControl::sort_clips()
{
	std::sort(m_Clips.begin(), m_Clips.end(), clip_compare);
}

//----------------------------------------------------------------------------
// Adjust size of control based on expanded and categories of clips
//----------------------------------------------------------------------------
void chnlChannelControl::compute_size()
{
	//count_categories();
	//if (m_bExpanded)
	//{
	//	int num_cats = this.m_Categories.Count + 1;
	//	this.Height = 4 + num_cats * c_ChannelHeight;  
	//}
	//else 

	//wxSize size(-1, 4 + c_ChannelHeight);
	wxSize size(this->get_full_width(), 32);
	this->SetMinSize(size);
	this->SetSize(size);
}

//----------------------------------------------------------------------------
// Get rectangle to represent the given clip
//----------------------------------------------------------------------------
void chnlChannelControl::get_rectangle(const chnlChannelClip& i_Clip, 
									   int &o_X, int &o_Y, int &o_Width, int &o_Height)
{
	o_X = get_position_for_time(i_Clip.GetBeginTime());
	o_Width = (int) (i_Clip.GetDuration() * m_TimeScale);

	// Make sure rectangle is at least one pixel wide
	if (o_Width < 1) o_Width = 1;
	o_Y = 0; //get_channel_root(clip); // channel root used when channels are expandable
	o_Y += 4;
	o_Height = 24;
}

//----------------------------------------------------------------------------
// Set the tool tip text for this channel
//----------------------------------------------------------------------------
void chnlChannelControl::set_tip_description()
{
	bool edge = false;

	//	if no interaction is going on then see if mouse is hovering over a clip
	//
	if (m_Interaction == e_None)
	{
		chnlChannelClip *pClip = pick_clip(m_CurX, m_CurY, edge);
		if (pClip)
		{
			this->SetToolTip( pClip->GetHoverDescription() );
		}
		else
		{
			this->SetToolTip( "No Clip" );
		}
	}
	else if (m_pSelectedClip)
	{
		// an interaction is going on so use the selected clip
		//
		this->SetToolTip(m_pSelectedClip->GetInteractionDescription(
			m_pSelectedClip->GetInteractStart(), m_pSelectedClip->GetInteractDuration()));
	}
}

//----------------------------------------------------------------------------
// Find clip at given x,y coordinates, returns NULL if no clip
//----------------------------------------------------------------------------
chnlChannelClip* chnlChannelControl::pick_clip(int i_eX, int i_eY, bool &o_Edge)
{
	const int c_EdgeTolerance = 2;

	o_Edge = false;		
	const int num_clips = m_Clips.size();
	for (int ci=0; ci<num_clips; ++ci)
	{
		chnlChannelClip *pClip = m_Clips[ci];

		int chnl_root = 0; // get_channel_root(clip);
		int x = get_position_for_time(pClip->GetBeginTime());
		int width = (int) (pClip->GetDuration() * m_TimeScale);

		// Special check for circle in short ki_eY drivers
		if (pClip->GetDuration() < c_ShortKeyDuration)
		{
			int circy = i_eY - (chnl_root+4);
			int circx = i_eX - (x-c_KeyCircleRadius);
			if ((circy >= 0) && (circy <= 2*c_KeyCircleRadius) &&
				(circx >= 0) && (circx <= 2*c_KeyCircleRadius))
			{
				return pClip;
			}
		}

		if (   (i_eY >= chnl_root) 
			&& (i_eY - chnl_root <= c_ChannelHeight))
		{
			if 	(   (i_eX >= x) 
				 && (i_eX - x <= (width + c_EdgeTolerance)))
			{
				// Clips must have a substantial length in time before
				// it makes sense to control the length with the mouse.
				if (pClip->GetDuration() >= c_MinimumInteractionDuration)
				{
					// check for right edge selection (for duration interaction)
					int hw = width / 2;
					if (hw > c_EdgeTolerance) hw = c_EdgeTolerance;
					int dx = i_eX - x - width;
					if (dx > -hw && dx < hw)
					{
						m_ResizingType = e_Right;
						o_Edge = true;
					}
				}

				// return clip
				return pClip;
			}
			if (   (i_eX >= (x - c_EdgeTolerance))
				&& (i_eX <= x))
			{
				// Clips must have a substantial length in time before
				// it makes sense to control the length with the mouse.
				if (pClip->GetDuration() >= c_MinimumInteractionDuration)
				{
					// check for left edge selection (for duration interaction)
					int hw = width / 2;
					if (hw > c_EdgeTolerance) hw = c_EdgeTolerance;
					int dx = x - i_eX;
					if (dx > -hw && dx < hw)
					{
						m_ResizingType = e_Left;
						o_Edge = true;
					}
				}

				// return clip
				return pClip;
			}
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnPaint(wxPaintEvent &WXUNUSED(event))
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

	dc.SetPen( wxPen(*wxLIGHT_GREY, 1, wxSOLID) );
	dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill

	// add our custom border
	wxRect border_rect = this->GetClientRect();
	//wxRect border_rect = this->GetRect();
	border_rect.x += c_EdgeOffset;
	border_rect.width -= 2*c_EdgeOffset;
	// should really be a 3D border style
	dc.DrawRectangle(border_rect);

	const int num_clips = m_Clips.size();
	if (num_clips > 0)
	{
		int x, width, y, height;

		// Draw each clip
		//
		wxPen border_pen( *wxBLACK, 1, wxSOLID);
		wxPen highlight_pen( *wxRED, 2, wxSOLID);
		wxFont text_font( 8, wxSWISS, wxNORMAL, wxNORMAL );
		dc.SetFont(text_font);

		const chnlChannelClip *prev_clip = NULL;
		for (int ci=0; ci<num_clips; ++ci)
		{
			const chnlChannelClip *pClip = m_Clips[ci];
			DBG_ASSERT0(pClip, "Assuming non-null channel clip pointers");

			this->get_rectangle(*pClip, x, y, width, height);
			int icon_center = c_IconCenter + y;
			wxBrush fill_brush(pClip->GetFillColor(), wxSOLID);
			dc.SetPen( border_pen );
			dc.SetBrush( fill_brush );

			if (pClip->GetDuration() < c_ShortKeyDuration)
			{
				//if (rect.IntersectsWith(e.ClipRectangle)
				//	|| circle.IntersectsWith(e.ClipRectangle))
				{
					dc.DrawRectangle(x, y, width, height);
					dc.DrawEllipse(x-c_KeyCircleRadius, y, 2*c_KeyCircleRadius, 2*c_KeyCircleRadius);
				}
			}
			else
			{
				// Longer, non-key drivers are rendered with rectangles
				//if (rect.IntersectsWith(e.ClipRectangle))
				{
					// Draw rectangle with black border and fill color inside
					dc.DrawRectangle(x, y, width, height);
					
					// Center the text in the clip's rectangle
					wxSize text_size = dc.GetTextExtent(pClip->GetName());
					float center_off = (width - text_size.GetWidth()) * 0.5f;
					if (center_off > 0)
					{
						// Draw name string inside clip's rectangle
						dc.DrawText(pClip->GetName(), x + center_off, y + 4);
					}
				}

			}
	
			// Previous clip is valid only if same category
			const chnlChannelClip *pPrev = prev_clip;
			//if ((pPrev != NULL) && (pPrev->GetCategory() != pClip->GetCategory()))
			//	pPrev = NULL;
			int prev_end = c_EdgeOffset;
			if (pPrev != NULL)
				prev_end = get_position_for_time(pPrev->GetEndTime());


			// draw blending icon
			dc.SetPen( *wxTRANSPARENT_PEN ); // turn off outlining
			switch (pClip->GetBlendType())
			{
				default:
					break;
				case chnlChannelClip::e_NoBlend:
				{
					dc.SetBrush(wxBrush(*wxBLACK, wxSOLID));
					dc.DrawRectangle(x-2, icon_center-2, 4, 4);
					break;
				}
				case chnlChannelClip::e_BlendTime:
				{
					int blend = (int)(pClip->GetBlendTime() * m_TimeScale);
					
					wxPoint points[3];
					points[0].x = x-blend+1; points[0].y = icon_center;
					points[1].x = x; points[1].y = icon_center-10;
					points[2].x = x; points[2].y = icon_center+10;

					dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
					dc.DrawPolygon(3, points);
					break;
				}
				case chnlChannelClip::e_Previous:
				{
					wxPoint points[3];
					points[0].x = prev_end+1; points[0].y = icon_center;
					points[1].x = x; points[1].y = icon_center-10;
					points[2].x = x; points[2].y = icon_center+10;

					dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
					dc.DrawPolygon(3, points);
					break;
				}
				case chnlChannelClip::e_Overwrite:
				{
					dc.SetBrush(wxBrush(*wxLIGHT_GREY, wxSOLID));
					dc.DrawRectangle(prev_end+2, icon_center-11, x-prev_end-2, height-2);
					break;
				}
				case chnlChannelClip::e_SmoothBlend:
				{
					int weight = (x - prev_end) / 3;
					if (weight < 1) weight = 2;

					wxPoint points[4];
					points[0].x = prev_end+2; points[0].y = icon_center+8;
					points[1].x = prev_end+weight; points[1].y = icon_center+8;
					points[2].x = x-weight; points[2].y = icon_center-4;
					points[3].x = x-2; points[3].y = icon_center-4;

					dc.SetPen(wxPen(*wxLIGHT_GREY, 4, wxSOLID));
					dc.DrawSpline(4, points);
					break;
				}
			}

			// Draw icon for end blending (restore or remain)
			wxBrush restore_brush((pClip->GetRestore() ? *wxBLACK : *wxLIGHT_GREY), wxSOLID);
			dc.SetPen( border_pen );
			dc.SetBrush(restore_brush);
			dc.DrawRectangle(x+width-2, 16+y, 4, 6);

			// Remember previous clip
			prev_clip = pClip;
		}

		// Highlight selected clips by drawing the interaction
		// rectangles for each clip
		if (!m_SelectedClips.empty())
		{
			dc.SetPen(highlight_pen);
			dc.SetBrush(*wxTRANSPARENT_BRUSH); // turn off fill
			std::set<chnlChannelClip*>::const_iterator it;
			for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
			{
				const chnlChannelClip *sel_clip = (*it);

				int ix = get_position_for_time(sel_clip->GetInteractStart());
				int iw = (int) (sel_clip->GetInteractDuration() * m_TimeScale);

				// Make sure rectangle is at least one pixel wide
				if (iw < 1) iw = 1;

				dc.DrawRectangle(ix, sel_clip->GetInteractRoot()+4, iw, 24);
			}
		}
	}

	// Draw guidelines on top of the channels
	chnlTimelineGuideUtil::PaintGuides(dc, this->GetClientRect().GetHeight());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnMouseDown(wxMouseEvent &i_Event)
{
//	DBG_LOG2("On MouseDown: %d %d", i_Event.GetX(), i_Event.GetY());
	
	//if (!this.TopLevelControl.ContainsFocus)
	//	return;

	bool edge = false;
	chnlChannelClip *pPick = this->pick_clip(i_Event.GetX(), i_Event.GetY(), edge);

	// SHIFT means append to selection
	bool append_selection = i_Event.ShiftDown();

	// If the clip is already selected, then use append in order
	// to pull it to the main selection, but keep the others still
	// selected for multiple clip movement.
	if (m_SelectedClips.find(pPick) != m_SelectedClips.end())
		append_selection = true;

	this->SelectClip(pPick, append_selection);
	
	if (m_pInteractionCallback != NULL)
	{
		m_pInteractionCallback->ClipSelected(this, pPick, append_selection);
	}

	if (pPick)
	{
		//	if this channel is locked, don't let any driver change.
		if (m_bLocked) return;

		// Don't let the right mouse button move the clip
		if (i_Event.GetButton() != wxMOUSE_BTN_LEFT)
			return;

		m_Interaction = (edge) ? e_Resizing : e_Moving;
		BeginInteraction();
		m_IX = i_Event.GetX();
		m_bInteractionConfirmed = false;
	}
	i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnMouseMove(wxMouseEvent &i_Event)
{
	// keep current mouse position for hovering
	m_CurX = i_Event.GetX(); 
	m_CurY = i_Event.GetY();
	
	// set the tool tip all the time in wxWidgets
	set_tip_description();

	// Act on interaction state
	if (m_Interaction == e_Moving)
	{
		int dx = i_Event.GetX() - m_IX;
		
		// Wait until there is a substantial movement of the mouse before
		// actuall altering the driver.
		if (::abs(dx) > c_PixelMoveThreshold)
			m_bInteractionConfirmed = true;

		if (m_bInteractionConfirmed)
		{
			float time_delta = (float)(dx / m_TimeScale);
			InteractMoveSelectedClips(time_delta);
			if (m_pInteractionCallback != NULL)
			{
				if ( dx != 0 )
				{
					m_pInteractionCallback->ClipMoved(this, time_delta);
				}
			}

			this->Refresh();
		}
	}
	else if (m_Interaction == e_Resizing)
	{
		int dx = i_Event.GetX() - m_IX;

		// Wait until there is a substantial movement of the mouse before
		// actuall altering the driver.
		if (::abs(dx) > c_PixelMoveThreshold)
			m_bInteractionConfirmed = true;

		if (m_bInteractionConfirmed)
		{
			float interact_start = m_pSelectedClip->GetInteractStart();
			float interact_duration = m_pSelectedClip->GetInteractDuration();
			if (m_ResizingType == e_Right)
			{
				interact_duration =  m_pSelectedClip->GetDuration() + dx / m_TimeScale;
			}
			else
			{
				float endtime = m_pSelectedClip->GetEndTime();
				interact_start = m_pSelectedClip->GetBeginTime() + dx / m_TimeScale;
				if (interact_start < 0) interact_start = 0.0f;

				interact_duration = endtime - interact_start;
			}

			// snap short lengths to 0 length, special key mode for driver
			if (interact_duration < 0.1f) 
				interact_duration = 0.0f;

			interact_duration = snap_time(interact_duration, m_SnapInterval);

			m_pSelectedClip->SetInteraction(interact_start, interact_duration);
			
			if (m_pInteractionCallback != NULL)
			{
				m_pInteractionCallback->ClipResized(this);
			}
			this->Refresh();
		}
	}
	else
	{
		bool edge = false;
		chnlChannelClip *pPick = pick_clip(i_Event.GetX(), i_Event.GetY(), edge);
		if (pPick)
			this->SetCursor( wxCursor( (edge) ? wxCURSOR_SIZEWE : wxCURSOR_HAND ) );
		else
			this->SetCursor( *wxSTANDARD_CURSOR );
	}
	i_Event.Skip();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnMouseUp(wxMouseEvent &i_Event)
{
	if (m_Interaction == e_Moving || 
		m_Interaction == e_Resizing)
	{
		if (m_bInteractionConfirmed)
		{
			FinishInteraction();
			if (m_pInteractionCallback != NULL)
			{
				m_pInteractionCallback->ClipMoveFinished(this);
			}
			this->Refresh();
		}
		else 
		{
			// If the interaction was not confirmed, then
			// this means it was a click on the selected
			// driver. 
			if (!i_Event.ShiftDown()) 
			{					
				// If SHIFT was not held and more than one
				// driver was selected, this means reduce the
				// selection to the single driver (append = false)
				this->SelectClip(m_pSelectedClip);
				if (m_pInteractionCallback != NULL)
				{
					bool append_selection = false;
					m_pInteractionCallback->ClipSelected(this, m_pSelectedClip, append_selection);
				}
			}
		}
	}
	//else if ((e.Y >= c_IconCenter-6) && (e.Y <= c_IconCenter+6) &&
	//	(e.X >= 0) && (e.X  <= 12))
	//{ 
	//	// mouse up on +/- sign
	//	this.Expanded = (!this.Expanded);
	//}

	m_Interaction = e_None;
	i_Event.Skip();

	// When selecting drivers, the interface for the driver properties
	// is created which takes away the focus. So, we need to take it back.
	this->SetFocus();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnSize(wxSizeEvent &i_Event)
{
	// Invalidate the whole rectangle
	this->Refresh();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnContextMenu(wxContextMenuEvent& i_Event)
{
	if (m_pSelectedClip)
	{
		wxPoint point = i_Event.GetPosition();
		// If from keyboard
		if (point.x == -1 && point.y == -1) 
		{
			wxSize size = GetSize();
			point.x = size.x / 2;
			point.y = size.y / 2;
		} 
		else 
		{
			point = ScreenToClient(point);
		}

		wxMenu menu;
		menu.Append(MENU_SHOW_PROPERTIES, _T("Show &Properties"));
		menu.Append(MENU_SELECTICON, _T("Select &Icon"));
		menu.Append(MENU_CUT, _T("Cut"));
		menu.Append(MENU_COPY, _T("Copy"));
		menu.Append(MENU_PASTE, _T("Paste"));
		PopupMenu(&menu, point.x, point.y);
	}

	else
	{
		wxPoint point = i_Event.GetPosition();
		// If from keyboard
		if (point.x == -1 && point.y == -1) 
		{
			wxSize size = GetSize();
			point.x = size.x / 2;
			point.y = size.y / 2;
		} 
		else 
		{
			point = ScreenToClient(point);
		}

		wxMenu menu;
		menu.Append(MENU_PASTE, _T("Paste"));
		PopupMenu(&menu, point.x, point.y);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnDoubleClick(wxMouseEvent& i_Event)
{
	if (m_pSelectedClip)
		m_pSelectedClip->ShowProperties();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnShowProperties(wxCommandEvent& i_Event)
{
	if (m_pSelectedClip)
		m_pSelectedClip->ShowProperties();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnSelectIcon(wxCommandEvent& i_Event)
{
	if (m_pSelectedClip)
		m_pSelectedClip->SelectIcon();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnCut(wxCommandEvent& i_Event)
{
	if (m_pSelectedClip)
	{
		chnlOperations::CutDrivers();
		DBG_WARNING0("Cut this driver");
	}

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnCopy(wxCommandEvent& i_Event)
{
	if (m_pSelectedClip)
	{
		chnlOperations::CopyDrivers();
		DBG_WARNING0("Copy this driver");
	}

}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::OnPaste(wxCommandEvent& i_Event)
{	
	chnlOperations::PasteDrivers();
	DBG_WARNING0("Paste this driver");	
}
//--------------------------------------------------------------------
// Called when a clip of ours changes
//--------------------------------------------------------------------
//virtual 
void chnlChannelControl::ClipChanged(chnlChannelClip*)
{
	this->Refresh();
}

#endif // USE_WXWIDGETS
