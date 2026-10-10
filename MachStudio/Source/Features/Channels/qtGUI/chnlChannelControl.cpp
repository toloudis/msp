/****************************************************************************\
**	chnlChannelControl.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlChannelControl.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlOperations.hpp"
#include "Features/Channels/qtGUI/chnlTimelineGuideUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/ma/maFunctions.hpp"
#include "Graphics/g3d/g3dConstants.hpp"

#include <algorithm>

#ifdef USE_QT

#include <QContextMenuEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>


//============================================================================
//============================================================================
namespace
{
	const int c_IconCenter = 12;
	const int c_ChannelHeight = 28; // 24 height + 4 buffer on each side
	// To display "keys" better, short duration drivers are represented differently.
	//const float c_ShortKeyDuration = (1.0f / 15.0f); // just one frame, or short period?
	const maTime c_ShortKeyDuration = maTime::FromFrame(1, 15);
	// If clips are less than this length, turn off the mouse resize interaction
	//const float c_MinimumInteractionDuration = 0.1f; 
	const maTime c_MinimumInteractionDuration = maTime::FromFrame(1, 10);
	// Short drivers have circle added, this defines radius in pixels of this circle
	const int c_KeyCircleRadius = 4; 
	// amount to move mouse before interaction is confirmed
	const int c_PixelMoveThreshold = 2; 
	// amount to round off the corners of the driver box
	const int c_RoundCornerRadius = 4;


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

	//--------------------------------------------------------------------
	// Rectangles cover width x height pixels like wxWidgets: a 1 pixel
	//	outline is drawn inside of that area.
	//--------------------------------------------------------------------
	void draw_rect(QPainter& io_DC, int i_X, int i_Y, int i_Width, int i_Height)
	{
		if (io_DC.pen().style() == Qt::NoPen)
			io_DC.drawRect(i_X, i_Y, i_Width, i_Height);
		else
			io_DC.drawRect(i_X, i_Y, qMax(i_Width-1, 0), qMax(i_Height-1, 0));
	}

	void draw_rounded_rect(QPainter& io_DC, int i_X, int i_Y, int i_Width, int i_Height, int i_Radius)
	{
		io_DC.drawRoundedRect(QRectF(i_X, i_Y, qMax(i_Width-1, 0), qMax(i_Height-1, 0)), i_Radius, i_Radius);
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
//--------------------------------------------------------------------
chnlChannelControl::chnlChannelControl(QWidget* i_pParent, bool i_bLocked)
:	QWidget(i_pParent),
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
	// Hovering shows the clip's description and the interaction cursor
	this->setMouseTracking(true);
	this->setFocusPolicy(Qt::ClickFocus);
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
	this->update();
}

//--------------------------------------------------------------------
// Add Clip to list - this control takes ownership of the clip
//--------------------------------------------------------------------
void chnlChannelControl::AddClip(chnlChannelClip* i_pClip)
{
	m_Clips.push_back(i_pClip);

	// register callback for when clip changes
	i_pClip->SetChangedCallback( this );

	this->sort_clips();
	this->compute_size();
	this->update();
}

//--------------------------------------------------------------------
// Selection functions
//--------------------------------------------------------------------
void chnlChannelControl::ClearSelection()
{
	this->SelectClip(NULL);
}
void chnlChannelControl::DeselectClip(chnlChannelClip* i_pClip)
{
	if (i_pClip != NULL)
	{
		this->m_SelectedClips.erase(i_pClip);

		if (m_pSelectedClip == i_pClip)
			m_pSelectedClip = NULL;
	}
	this->update();
}
void chnlChannelControl::SelectClip(chnlChannelClip* i_pClip, bool i_bAppend)
{
	m_pSelectedClip = i_pClip; 
	if (!i_bAppend)
	{
		clear_selected(this->m_SelectedClips);
	}
	if (i_pClip != NULL)
	{
		this->m_SelectedClips.insert(i_pClip);
	}
	this->update();
		
	// Call virtual "Select" function to allow driver to 
	//  handle this event
	if (m_pSelectedClip != NULL)
	{
		m_pSelectedClip->Select();
	}
}

void chnlChannelControl::SelectClips(float i_StartTime, float i_EndTime)
{
	maTime start_time = maTime::FromSeconds(i_StartTime);
	maTime end_time = maTime::FromSeconds(i_EndTime);

	const int num_clips = m_Clips.size();
	for (int ci=0; ci<num_clips; ++ci)
	{
		chnlChannelClip *pClip = m_Clips[ci];

		int chnl_root = 0; // get_channel_root(clip);

		if (((pClip->GetBeginTime() >= start_time) && (pClip->GetBeginTime() <= end_time))
			//|| ((pClip->GetEndTime() >= i_StartTime) && (pClip->GetEndTime() <= i_EndTime))
			)
		{
			SelectClip(pClip, true);	// append
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
			maTime start_time = (*min_it)->GetBeginTime();
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
	DBG_ASSERT(i_Index>=0 && i_Index<m_Clips.size(), "GetClip out of range " << i_Index << " < " << m_Clips.size());
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
	
	if(chnlChannelControl::GetLocked()) return;
	std::set<chnlChannelClip*>::iterator it;
	
	maTime time_delta = maTime::FromSeconds(i_TimeDelta);
	for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
	{
		chnlChannelClip *sel_clip = (*it);
		if( ((sel_clip->GetBeginTime() + (time_delta)) <  tmlnTimeLine::GetMinimum())
			|| ((sel_clip->GetBeginTime() + (time_delta)) >  tmlnTimeLine::GetMaximum()))
		{
			return;
		}
	}
	for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
	{
		chnlChannelClip *sel_clip = (*it);
		maTime interact_start = sel_clip->GetBeginTime() + time_delta;

		// I am not sure what to enforce as the minimum time anymore. 
		// Maybe we can allow the begin time to go below the minimum as long as
		// the end time doesn't?
		maTime min_time = maFunctions::Lowest(tmlnTimeLine::GetMinimum(), sel_clip->GetBeginTime());
		if (interact_start < min_time) interact_start = min_time;

		interact_start = maTime::FromSeconds(snap_time(interact_start.AsSeconds(), m_SnapInterval));
		sel_clip->SetInteraction( interact_start, sel_clip->GetInteractDuration());
	}

	if (!m_SelectedClips.empty())
		this->update();
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
		this->update();
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
	this->update();
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

	this->setFixedSize(this->get_full_width(), 32);
}

//----------------------------------------------------------------------------
// Get rectangle to represent the given clip
//----------------------------------------------------------------------------
void chnlChannelControl::get_rectangle(const chnlChannelClip& i_Clip, 
									   int &o_X, int &o_Y, int &o_Width, int &o_Height)
{
	o_X = get_position_for_time(i_Clip.GetBeginTime().AsSeconds());
	o_Width = (int) (i_Clip.GetDuration().AsSeconds() * m_TimeScale);

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
			this->setToolTip( tqtQString(pClip->GetHoverDescription()) );
		}
		else
		{
			this->setToolTip( "No Clip" );
		}
	}
	else if (m_pSelectedClip != NULL)
	{
		// an interaction is going on so use the selected clip
		//
		std::string description = m_pSelectedClip->GetInteractionDescription(
			m_pSelectedClip->GetInteractStart(), m_pSelectedClip->GetInteractDuration());
		this->setToolTip( tqtQString(description) );
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
		int x = get_position_for_time(pClip->GetBeginTime().AsSeconds());
		int width = (int) (pClip->GetDuration().AsSeconds() * m_TimeScale);

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
void chnlChannelControl::paintEvent(QPaintEvent*)
{
	QPainter dc(this);
	dc.fillRect(this->rect(), this->palette().window());

	dc.setPen( QPen( QColor(130,130,150), 1 ) );
	dc.setBrush( Qt::NoBrush );

	// add our custom border
	draw_rect(dc, c_EdgeOffset, 0, this->width() - 2*c_EdgeOffset, this->height());

	const int num_clips = m_Clips.size();
	if (num_clips > 0)
	{
		int x, width, y, height;

		// Draw each clip
		//
		const QPen border_pen( Qt::black, 1 );
		const QPen highlight_pen( Qt::red, 2 );
		QFont text_font = this->font();
		text_font.setPointSize(8);
		text_font.setBold(true);
		dc.setFont(text_font);
		const QFontMetrics metrics = dc.fontMetrics();

		const chnlChannelClip *prev_clip = NULL;
		for (int ci=0; ci<num_clips; ++ci)
		{
			const chnlChannelClip *pClip = m_Clips[ci];
			DBG_ASSERT(pClip != NULL, "Assuming non-null channel clip pointers");
			if (pClip == NULL) continue;	// skip this clip

			this->get_rectangle(*pClip, x, y, width, height);
			int icon_center = c_IconCenter + y;

			// Previous clip
			const chnlChannelClip *pPrev = prev_clip;
			int prev_end = c_EdgeOffset;
			if (pPrev != NULL)
				prev_end = get_position_for_time(pPrev->GetEndTime().AsSeconds());

			// draw blending icon first so that main icon overlaps it
			dc.setPen( Qt::NoPen );	// turn off outlining
			const QColor blend_color(200,210,200);
			switch (pClip->GetBlendType())
			{
				default:
					break;
				case chnlChannelClip::e_NoBlend:
				{
					dc.setBrush(blend_color);
					draw_rect(dc, x-2, icon_center-2, 4, 4);
					break;
				}
				case chnlChannelClip::e_BlendTime:
				{
					int blend = (int)(pClip->GetBlendTime().AsSeconds() * m_TimeScale);
					QPoint points[3] = { QPoint(x-blend+1, icon_center), QPoint(x, icon_center-10), QPoint(x, icon_center+10) };
					dc.setBrush(blend_color);
					dc.drawPolygon(points, 3);
					break;
				}
				case chnlChannelClip::e_Previous:
				{
					QPoint points[3] = { QPoint(prev_end+1, icon_center), QPoint(x, icon_center-10), QPoint(x, icon_center+10) };
					dc.setBrush(blend_color);
					dc.drawPolygon(points, 3);
					break;
				}
				case chnlChannelClip::e_Overwrite:
				{
					dc.setBrush(blend_color);
					draw_rect(dc, prev_end+2, icon_center-11, x-prev_end-2, height-2);
					break;
				}
				case chnlChannelClip::e_SmoothBlend:
				{
					int weight = (x - prev_end) / 3;
					if (weight < 1) weight = 2;

					// An S curve easing from the previous clip into this one
					QPainterPath path(QPointF(prev_end+2, icon_center+8));
					path.cubicTo(QPointF(prev_end+weight, icon_center+8),
								 QPointF(x-weight, icon_center-4),
								 QPointF(x-2, icon_center-4));
					dc.setPen(QPen(blend_color, 4));
					dc.setBrush(Qt::NoBrush);
					dc.drawPath(path);
					break;
				}
			}

			// Then draw actual icon for the driver
			dc.setPen( border_pen );
			dc.setBrush( pClip->GetFillColor() );

			if (pClip->GetDuration() < c_ShortKeyDuration)
			{
				draw_rect(dc, x, y, width, height);
				dc.drawEllipse(x-c_KeyCircleRadius, y, 2*c_KeyCircleRadius-1, 2*c_KeyCircleRadius-1);
			}
			else
			{
				// Longer, non-key drivers are rendered with rectangles,
				// with a black border and fill color inside
				draw_rounded_rect(dc, x, y, width, height, c_RoundCornerRadius);

				// Try to give it some highlights
				dc.setPen( Qt::lightGray );
				dc.drawLine(x+c_RoundCornerRadius,y+1,x+width-c_RoundCornerRadius,y+1);
				dc.drawLine(x+1,y+c_RoundCornerRadius,x+1,y+height-c_RoundCornerRadius);
				dc.setPen( Qt::gray );
				dc.drawLine(x+c_RoundCornerRadius,y+height-2,x+width-c_RoundCornerRadius,y+height-2);
				dc.drawLine(x+width-2,y+c_RoundCornerRadius,x+width-2,y+height-c_RoundCornerRadius);
				dc.setPen( border_pen );

				// Center the text in the clip's rectangle
				const QString clip_name = tqtQString(pClip->GetName());
				float center_off = (width - metrics.horizontalAdvance(clip_name)) * 0.5f;
				if (center_off > 0)
				{
					// Draw name string inside clip's rectangle
					dc.drawText(x + (int)center_off, y + 4 + metrics.ascent(), clip_name);
				}
			}

			// Draw icon for end blending (restore or remain)
			dc.setPen( border_pen );
			dc.setBrush( pClip->GetRestore() ? Qt::black : Qt::lightGray );
			draw_rect(dc, x+width-2, 16+y, 4, 6);

			// Remember previous clip
			prev_clip = pClip;
		}

		// Highlight selected clips by drawing the interaction
		// rectangles for each clip
		if (!m_SelectedClips.empty())
		{
			dc.setPen(highlight_pen);
			dc.setBrush(Qt::NoBrush); // turn off fill
			std::set<chnlChannelClip*>::const_iterator it;
			for (it = m_SelectedClips.begin(); it != m_SelectedClips.end(); ++it)
			{
				const chnlChannelClip *sel_clip = (*it);

				int ix = get_position_for_time(sel_clip->GetInteractStart().AsSeconds());
				int iw = (int) (sel_clip->GetInteractDuration().AsSeconds() * m_TimeScale);

				// Make sure rectangle is at least one pixel wide
				if (iw < 1) iw = 1;

				// short clips need regular rectangle, not rounded
				if (iw < 3)
					draw_rect(dc, ix, sel_clip->GetInteractRoot()+4, iw, 24);
				else
					draw_rounded_rect(dc, ix, sel_clip->GetInteractRoot()+4, iw, 25, c_RoundCornerRadius);
			}
		}
	}

	// Draw guidelines on top of the channels
	chnlTimelineGuideUtil::PaintGuides(dc, this->height());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::mousePressEvent(QMouseEvent* i_pEvent)
{
	const QPoint pos = i_pEvent->position().toPoint();
	const Qt::MouseButton button = i_pEvent->button();
	if (button != Qt::LeftButton && button != Qt::RightButton)
	{
		QWidget::mousePressEvent(i_pEvent);
		return;
	}

	bool edge = false;
	chnlChannelClip *pPick = this->pick_clip(pos.x(), pos.y(), edge);

	if (i_pEvent->modifiers() & Qt::ControlModifier)
	{
		// If CTRL is being held down, then toggle the clip's selection state
		bool append_selection = true; // CTRL is also an append operation
		if (pPick != NULL)
		{
			if (m_SelectedClips.find(pPick) == m_SelectedClips.end())
			{
				// Not already selected, select it now
				this->SelectClip(pPick, append_selection);
				if (m_pInteractionCallback != NULL)
				{
					m_pInteractionCallback->ClipSelected(this, pPick, append_selection);
				}
			}
			else
			{
				// Already selected, remove it from the list
				this->DeselectClip(pPick);
				if (m_pInteractionCallback != NULL)
				{
					m_pInteractionCallback->ClipDeselected(this, pPick);
				}
			}
		}
	}
	else
	{
		// SHIFT means append to selection
		bool append_selection = (i_pEvent->modifiers() & Qt::ShiftModifier) != 0;

		// If the clip is already selected, then use append in order
		// to pull it to the main selection, but keep the others still
		// selected for multiple clip movement.
		if (m_SelectedClips.find(pPick) != m_SelectedClips.end())
			append_selection = true;

		// If the user is holding down shift and didn't pick a clip,
		// then don't change the selection, they likely just missed what they were aiming at.
		// But, if nothing was picked and Shift was not held, then
		// call SelectClip with NULL
		if (pPick != NULL || !append_selection)
		{
			this->SelectClip(pPick, append_selection);

			if (m_pInteractionCallback != NULL)
			{
				m_pInteractionCallback->ClipSelected(this, pPick, append_selection);
			}
		}

		// Only the left button moves or resizes an unlocked clip
		if (pPick != NULL && !m_bLocked && button == Qt::LeftButton)
		{
			m_Interaction = (edge) ? e_Resizing : e_Moving;
			BeginInteraction();
			m_IX = pos.x();
			m_bInteractionConfirmed = false;
		}
	}

	// Clicks on a channel don't start a time selection in the panel behind it
	i_pEvent->accept();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	const QPoint pos = i_pEvent->position().toPoint();

	// keep current mouse position for hovering
	m_CurX = pos.x();
	m_CurY = pos.y();

	set_tip_description();

	// Act on interaction state
	if (m_Interaction == e_Moving)
	{
		int dx = pos.x() - m_IX;

		// Wait until there is a substantial movement of the mouse before
		// actually altering the driver.
		if (::abs(dx) > c_PixelMoveThreshold)
			m_bInteractionConfirmed = true;

		if (m_bInteractionConfirmed)
		{
			float time_delta = (float)(dx / m_TimeScale);
			if (m_pInteractionCallback != NULL && dx != 0)
			{
				m_pInteractionCallback->ClipMoved(this, time_delta);
			}
			this->update();
		}
	}
	else if (m_Interaction == e_Resizing)
	{
		int dx = pos.x() - m_IX;

		// Wait until there is a substantial movement of the mouse before
		// actually altering the driver.
		if (::abs(dx) > c_PixelMoveThreshold)
			m_bInteractionConfirmed = true;

		if (m_bInteractionConfirmed)
		{
			float interact_start = m_pSelectedClip->GetInteractStart().AsSeconds();
			float interact_duration = m_pSelectedClip->GetInteractDuration().AsSeconds();
			if (m_ResizingType == e_Right)
			{
				interact_duration =  m_pSelectedClip->GetDuration().AsSeconds() + dx / m_TimeScale;
			}
			else
			{
				float endtime = m_pSelectedClip->GetEndTime().AsSeconds();
				interact_start = m_pSelectedClip->GetBeginTime().AsSeconds() + dx / m_TimeScale;
				if (interact_start < 0) interact_start = 0.0f;

				interact_duration = endtime - interact_start;
			}

			// snap short lengths to 0 length, special key mode for driver
			if (interact_duration < 0.1f)
				interact_duration = 0.0f;

			interact_duration = snap_time(interact_duration, m_SnapInterval);

			m_pSelectedClip->SetInteraction(maTime::FromSeconds(interact_start),
											maTime::FromSeconds(interact_duration));

			if (m_pInteractionCallback != NULL)
			{
				m_pInteractionCallback->ClipResized(this);
			}
			this->update();
		}
	}
	else
	{
		bool edge = false;
		chnlChannelClip *pPick = pick_clip(pos.x(), pos.y(), edge);
		if (pPick)
			this->setCursor( (edge) ? Qt::SizeHorCursor : Qt::PointingHandCursor );
		else
			this->unsetCursor();
	}
	i_pEvent->accept();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::mouseReleaseEvent(QMouseEvent* i_pEvent)
{
	if (i_pEvent->button() != Qt::LeftButton)
	{
		QWidget::mouseReleaseEvent(i_pEvent);
		return;
	}

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
			this->update();
		}
		else if (!(i_pEvent->modifiers() & Qt::ShiftModifier))
		{
			// If the interaction was not confirmed, then this was a click
			// on the selected driver. If SHIFT was not held and more than
			// one driver was selected, this means reduce the selection to
			// the single driver (append = false)
			this->SelectClip(m_pSelectedClip);
			if (m_pInteractionCallback != NULL)
			{
				bool append_selection = false;
				m_pInteractionCallback->ClipSelected(this, m_pSelectedClip, append_selection);
			}
		}
	}

	m_Interaction = e_None;
	i_pEvent->accept();

	// When selecting drivers, the interface for the driver properties
	// is created which takes away the focus. So, we need to take it back.
	this->setFocus();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::mouseDoubleClickEvent(QMouseEvent* i_pEvent)
{
	if (i_pEvent->button() == Qt::LeftButton && m_pSelectedClip != NULL)
		m_pSelectedClip->ShowProperties();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlChannelControl::contextMenuEvent(QContextMenuEvent* i_pEvent)
{
	QPoint global_pos = i_pEvent->globalPos();
	if (i_pEvent->reason() == QContextMenuEvent::Keyboard)
		global_pos = this->mapToGlobal(this->rect().center());

	show_context_menu(global_pos);
}

//----------------------------------------------------------------------------
// Commands for the selected clip, or just paste if no clip is selected
//----------------------------------------------------------------------------
void chnlChannelControl::show_context_menu(const QPoint& i_GlobalPos)
{
	QMenu menu(this);
	if (m_pSelectedClip != NULL)
	{
		menu.addAction("Show &Properties", [this]() { if (m_pSelectedClip) m_pSelectedClip->ShowProperties(); });
		menu.addAction("Select &Icon", [this]() { if (m_pSelectedClip) m_pSelectedClip->SelectIcon(); });
		menu.addSeparator();
		menu.addAction("Blend None", []() { chnlOperations::SetSelectedDriversBlend(tmlnDriver::e_NoBlending); });
		menu.addAction("Blend Previous", []() { chnlOperations::SetSelectedDriversBlend(tmlnDriver::e_Previous); });
		menu.addAction("Blend Smooth", []() { chnlOperations::SetSelectedDriversBlend(tmlnDriver::e_SmoothBlend); });
		menu.addSeparator();
		menu.addAction("Cut", []() { chnlOperations::CutDrivers(); });
		menu.addAction("Copy", []() { chnlOperations::CopyDrivers(); });
	}
	menu.addAction("Paste", []() { chnlOperations::PasteDrivers(); });
	menu.exec(i_GlobalPos);
}

//--------------------------------------------------------------------
// Called when a clip of ours changes
//--------------------------------------------------------------------
//virtual
void chnlChannelControl::ClipChanged(chnlChannelClip*)
{
	this->update();
}

#endif // USE_QT
