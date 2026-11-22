/****************************************************************************\
**	chnlTimeLabel.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Channels/qtGUI/chnlTimeLabel.hpp"

#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/Ma/maTime.hpp"

#include <sstream>
#include <iomanip>

#ifdef USE_QT

#include <QPainter>

//============================================================================
//============================================================================
namespace
{
	std::string convert_num(int i_Num, bool i_bDisplayMinutes)
	{
		std::ostringstream str;
		if (i_bDisplayMinutes)
		{
			str << std::setw(2) << std::setfill('0') << (i_Num / 60) << ":"
				<< std::setw(2) << std::setfill('0') << (i_Num % 60);
		}
		else
			str << i_Num;
		return str.str();
	}

	//--------------------------------------------------------------------
	// Draw text centered horizontally on i_X with its top at i_Y
	//--------------------------------------------------------------------
	void draw_centered_text(QPainter &io_DC, const QString& i_Text, int i_X, int i_Y)
	{
		const QFontMetrics metrics = io_DC.fontMetrics();
		const int center_off = metrics.horizontalAdvance(i_Text) / 2;
		io_DC.drawText(i_X - center_off, i_Y + metrics.ascent(), i_Text);
	}

	void draw_num(QPainter &io_DC, int i_Num, int i_X, int i_Y, bool i_bDisplayMinutes)
	{
		draw_centered_text(io_DC, tqtQString(convert_num(i_Num, i_bDisplayMinutes)), i_X, i_Y);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chnlTimeLabel::chnlTimeLabel(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_FramesPerSecond(24),
	m_bDisplayFrames(false),
	m_bDisplayMinutes(false)
{
	this->compute_size();
}

//----------------------------------------------------------------------------
// SetFramesPerSecond
//----------------------------------------------------------------------------
void chnlTimeLabel::SetFramesPerSecond(int i_FPS)
{
	m_FramesPerSecond = i_FPS;
	this->update();
}

int chnlTimeLabel::GetFramesPerSecond() const
{
	return m_FramesPerSecond;
}

//----------------------------------------------------------------------------
// SetDisplayFrames - display numbers as seconds or frames
//----------------------------------------------------------------------------
void chnlTimeLabel::SetDisplayFrames(bool i_bDisplayFrames)
{
	m_bDisplayFrames = i_bDisplayFrames;
	this->update();
}

bool chnlTimeLabel::GetDisplayFrames() const
{
	return m_bDisplayFrames;
}

//----------------------------------------------------------------------------
// SetDisplayMinutes - display numbers as seconds or minutes:seconds
//----------------------------------------------------------------------------
void chnlTimeLabel::SetDisplayMinutes(bool i_bDisplayMinutes)
{
	m_bDisplayMinutes = i_bDisplayMinutes;
	this->update();
}

bool chnlTimeLabel::GetDisplayMinutes() const
{
	return m_bDisplayMinutes;
}

//----------------------------------------------------------------------------
// Virtual function called when time range or scale has changed
//----------------------------------------------------------------------------
void chnlTimeLabel::update_size()
{
	this->compute_size();
	this->update();
}

//----------------------------------------------------------------------------
// The label is as wide as the timeline
//----------------------------------------------------------------------------
void chnlTimeLabel::compute_size()
{
	this->setFixedSize(this->get_full_width(), 32);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chnlTimeLabel::paintEvent(QPaintEvent*)
{
	QPainter dc(this);
	dc.fillRect(this->rect(), this->palette().window());

	// Draw time hashes
	const QColor text_color = this->palette().windowText().color();
	QPen line_pen( text_color, 1 );
	QPen thick_pen( text_color, 3 );
	thick_pen.setCapStyle( Qt::FlatCap );

	int num_ticks = m_MaxTime-m_MinTime;	// how many ticks to draw
	int tick_group = 10;			// how often to have a bolder, larger tick
	float spacing = m_TimeScale;	// how far apart should the ticks be?
	float tick_time = 1.0f;			// how much time does a tick represent?

	bool bShowFrameTicks = (m_TimeScale > (3 * m_FramesPerSecond));
	if (bShowFrameTicks)
	{
		num_ticks = (m_MaxTime-m_MinTime) * m_FramesPerSecond;
		tick_group = m_FramesPerSecond;
		spacing = (m_TimeScale / (float)m_FramesPerSecond);
		tick_time = (1.0f / (float)m_FramesPerSecond);
	}

	int tick_shift = (m_MinTime < 0) ? ((int) (m_MinTime / tick_time - 0.5f)) :
									   ((int) (m_MinTime / tick_time + 0.5f));
	for (int i=0; i<=num_ticks; i++)
	{
		int pos = (int)(i * spacing + c_EdgeOffset);
		if ((i+tick_shift)%tick_group == 0)
		{
			dc.setPen(thick_pen);
			dc.drawLine(pos, 20, pos, 24);
		}
		else
		{
			dc.setPen(line_pen);
			dc.drawLine(pos, 21, pos, 24);
		}
	}

	// Always a fat tick at begin and end time
	int begin_pos = get_position_for_time(m_MinTime);
	int end_pos = get_position_for_time(m_MaxTime);
	dc.setPen(thick_pen);
	dc.drawLine(begin_pos, 20, begin_pos, 24);
	dc.drawLine(end_pos, 20, end_pos, 24);

	// Label time numbers
	dc.setPen(text_color);
	bool bDisplayMinutes = (m_bDisplayMinutes && !m_bDisplayFrames);
	QFont text_font = this->font();
	text_font.setPointSize(8);
	dc.setFont(text_font);

	int skip = (m_TimeScale > 25) ? 1 : 10;
	int mod = (m_bDisplayFrames) ? m_FramesPerSecond : 1;
	float last_time_to_number = m_MaxTime-(tick_time*0.5f); // have to stop numbering close to end time
	float first_time_to_number = m_MinTime+(tick_time*0.5f);
	int start_second = (int)first_time_to_number + skip;
	if (first_time_to_number < 0) start_second -= 1; // (int) cast moves towards zero
	for (int i=start_second; i<=last_time_to_number; i+=skip)
	{
		int pos = get_position_for_time((float)i);
		draw_num(dc, i*mod, pos, 0, bDisplayMinutes);
	}

	// Draw begin and end time in complete format - they don't always align
	// with an even second in order to use an "int" in the code above.
	std::string begin_time_string;
	tmlnTimeUtil::GetTimeString(maTime::FromSeconds(m_MinTime), begin_time_string);
	draw_centered_text(dc, tqtQString(begin_time_string), begin_pos, 0);

	std::string end_time_string;
	tmlnTimeUtil::GetTimeString(maTime::FromSeconds(m_MaxTime), end_time_string);
	draw_centered_text(dc, tqtQString(end_time_string), end_pos, 0);
}

#endif // USE_QT
