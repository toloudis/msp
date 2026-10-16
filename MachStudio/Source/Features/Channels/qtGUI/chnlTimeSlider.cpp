/****************************************************************************\
**	chnlTimeSlider.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlTimeSlider.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFunctions.hpp"

#ifdef USE_QT

#include <QMouseEvent>
#include <QPainter>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlTimeSlider::chnlTimeSlider(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_CurrentTime(0),
	m_bScrubbing(false)
{
	this->compute_size();
}

//----------------------------------------------------------------------------
// CurrentTime
//----------------------------------------------------------------------------
void chnlTimeSlider::SetCurrentTime(float i_Time)
{
	m_CurrentTime = i_Time;
	this->update();
}

float chnlTimeSlider::GetCurrentTime() const
{
	return m_CurrentTime;
}

//----------------------------------------------------------------------------
// Get the horizontal position of the current time
//----------------------------------------------------------------------------
int chnlTimeSlider::GetCurrentTimePosition()
{
	return get_position_for_time(m_CurrentTime);
}

//----------------------------------------------------------------------------
//	Get the time based on the horizontal position
//----------------------------------------------------------------------------
float chnlTimeSlider::GetTimeAtPosition(int i_ScreenXPos)
{
	return get_time_at_position(i_ScreenXPos);
}

//----------------------------------------------------------------------------
// Clear time ticks
//----------------------------------------------------------------------------
void chnlTimeSlider::ClearTimeTicks()
{
	m_TimeTicks.clear();
	this->update();
}

//----------------------------------------------------------------------------
// Add a time tick
//----------------------------------------------------------------------------
void chnlTimeSlider::AddTimeTick(float i_Time)
{
	m_TimeTicks.insert(i_Time);
	this->update();
}

//----------------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//----------------------------------------------------------------------------
void chnlTimeSlider::update_size()
{
	this->compute_size();
	this->update();
}

//----------------------------------------------------------------------------
// The slider is as wide as the timeline
//----------------------------------------------------------------------------
void chnlTimeSlider::compute_size()
{
	this->setFixedSize(this->get_full_width(), 16);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::paintEvent(QPaintEvent*)
{
	QPainter dc(this);
	dc.fillRect(this->rect(), this->palette().window());

	const int ht = this->height() - 4;

	// Outline the client rectangle (instead of using a windows border)
	dc.setPen( QPen( Qt::lightGray, 1 ) );
	dc.setBrush( Qt::NoBrush );
	dc.drawRect( 0, 0, this->width()-1, this->height()-1 );

	// Draw time ticks
	std::set<float>::const_iterator it;
	for (it = m_TimeTicks.begin(); it != m_TimeTicks.end(); ++it)
	{
		int pos = get_position_for_time(*it);
		dc.drawLine( pos, 0, pos, ht);
	}

	// Draw color triangle at current time, filled with firebrick red
	int pos = get_position_for_time(m_CurrentTime);
	int off = ht / 2;
	QPoint points[3] = { QPoint(pos, 0), QPoint(pos-off, ht), QPoint(pos+off, ht) };
	dc.setPen( QPen( Qt::black, 1 ) );
	dc.setBrush( QColor(178, 34, 34) );
	dc.drawPolygon( points, 3 );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::mousePressEvent(QMouseEvent* i_pEvent)
{
	if (i_pEvent->button() == Qt::LeftButton)
	{
		m_bScrubbing = true;
		tmlnTimeLine::SetIsScrubbing( true );
		handle_mouse_event( i_pEvent->position().toPoint().x() );
	}
	QWidget::mousePressEvent(i_pEvent);
}

//----------------------------------------------------------------------------
// m_bScrubbing is true if the mouse down event was on our control. We
// don't want to alter the timeline when the user is controlling the camera
// from the render window and moves over our control.
//----------------------------------------------------------------------------
void chnlTimeSlider::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	if (m_bScrubbing && (i_pEvent->buttons() & Qt::LeftButton))
		handle_mouse_event( i_pEvent->position().toPoint().x() );
	QWidget::mouseMoveEvent(i_pEvent);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::mouseReleaseEvent(QMouseEvent* i_pEvent)
{
	if (i_pEvent->button() == Qt::LeftButton)
		stop_scrubbing();
	QWidget::mouseReleaseEvent(i_pEvent);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::stop_scrubbing()
{
	m_bScrubbing = false;
	tmlnTimeLine::SetIsScrubbing( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeSlider::handle_mouse_event(int i_X)
{
	float time = get_time_at_position(i_X);
	maFunctions::Clamp(time, m_MinTime, m_MaxTime);
	tmlnTimeUtil::AdjustTimeToFrame( time );
	if (m_CurrentTime != time)
	{
		m_CurrentTime = time;
		this->update();

		// trigger the callback
		this->NotifyValueChanged();
	}
}

#endif // USE_QT
