/*****************************************************************************
**  qtTimeSlider.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/qtGUI/qtTimeSlider.hpp"

#include "Support/ptm/qtGUI/ptmTimeEdit.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#ifdef USE_QT

namespace
{
	// Width of the time edit boxes
	const int c_TimeEditWidth = 80;

	//------------------------------------------------------------------------
	// Time edit box with a label under it
	//------------------------------------------------------------------------
	ptmTimeEdit* add_time_edit(QWidget* i_pParent, QHBoxLayout* i_pLayout, const char* i_pLabel)
	{
		QVBoxLayout* pColumn = new QVBoxLayout;
		pColumn->setSpacing(0);
		ptmTimeEdit* pEdit = new ptmTimeEdit(i_pParent);
		pEdit->setFixedWidth(c_TimeEditWidth);
		pEdit->setAlignment(Qt::AlignCenter);
		pColumn->addWidget(pEdit);
		QLabel* pLabel = new QLabel(i_pLabel, i_pParent);
		pColumn->addWidget(pLabel, 0, Qt::AlignHCenter);
		i_pLayout->addLayout(pColumn);
		return pEdit;
	}
}

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
qtTimeSlider::qtTimeSlider( QWidget* parent )
: QWidget( parent )
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(5, 5, 5, 5);
	bSizer1->setSpacing(5);

	// Min Time
	m_floatEdit_MinTime = add_time_edit(this, bSizer1, "Min time");
	m_floatEdit_MinTime->SetTimeValue(tmlnTimeLine::GetMinimum());
	m_floatEdit_MinTime->SetValueChangedCallback([this]() { MinTimeChanged(); });

	// Slider
	m_slider1 = new tqcRangedFloat( this );
	m_slider1->ShowValue( false );
	m_slider1->SetRange( tmlnTimeLine::GetMinimum().AsSeconds(),tmlnTimeLine::GetMaximum().AsSeconds() );
	m_slider1->SetRestrictFlag(true);
	m_slider1->SetNumTicks( (short)(tmlnTimeLine::GetMaximum().AsSeconds() * tmlnTimeLine::GetFPS()) );
	m_slider1->SetValue(tmlnTimeLine::GetValue().AsSeconds());
	m_slider1->SetValueChangedCallback([this]() { SliderChanged(); });
	QObject::connect(m_slider1->GetSlider(), &QSlider::sliderPressed, [this]() { SliderThumbDown(); });
	QObject::connect(m_slider1->GetSlider(), &QSlider::sliderReleased, [this]() { SliderThumbUp(); });
	bSizer1->addWidget( m_slider1, 1 );

	// Current Time
	m_floatEdit_CurrentTime = add_time_edit(this, bSizer1, "Current time");
	m_floatEdit_CurrentTime->SetTimeValue(tmlnTimeLine::GetValue());
	m_floatEdit_CurrentTime->SetValueChangedCallback([this]() { CurrentTimeChanged(); });

	// Max Time
	m_floatEdit_MaxTime = add_time_edit(this, bSizer1, "Max time");
	m_floatEdit_MaxTime->SetTimeValue(tmlnTimeLine::GetMaximum());
	m_floatEdit_MaxTime->SetValueChangedCallback([this]() { MaxTimeChanged(); });

	// Add this panel to the pane manager
	const char* c_Title = "Time Slider";
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(c_Title).Caption(c_Title).Show().Layer(2).Bottom());

	// Add this control as a time interest
	tmlnTimeLine::AddTimeInterest(this);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
qtTimeSlider::~qtTimeSlider()
{
	// Unregister time interest
	tmlnTimeLine::RemoveTimeInterest(this);
}

//--------------------------------------------------------------------
//	TimeChanged - timeline current time has changed
//--------------------------------------------------------------------
//virtual
void qtTimeSlider::TimeChanged( const maTime& i_Time )
{
	m_floatEdit_CurrentTime->SetTimeValue(tmlnTimeLine::GetValue());
	m_slider1->SetValue( tmlnTimeLine::GetValue().AsSeconds() );
}

//--------------------------------------------------------------------
//	TimeRangeChanged - timeline maximum time has changed
//--------------------------------------------------------------------
//virtual
void qtTimeSlider::TimeRangeChanged( const maTime& i_MinTime, const maTime& i_MaxTime )
{
	m_floatEdit_MinTime->SetTimeValue(i_MinTime);
	m_floatEdit_MaxTime->SetTimeValue(i_MaxTime);
	m_slider1->SetRange( i_MinTime.AsSeconds(), i_MaxTime.AsSeconds() );
	float num_frames = (i_MaxTime - i_MinTime).AsFrame( tmlnTimeLine::GetFPS() );
	if (num_frames < 0) num_frames = 0;
	m_slider1->SetNumTicks( (short)num_frames );
}

//--------------------------------------------------------------------
//	TimeFormatChanged - timeline time display format has changed
//--------------------------------------------------------------------
//virtual
void qtTimeSlider::TimeFormatChanged( int i_TimeFormat )
{
}

//--------------------------------------------------------------------
//	FrameRateChanged - timeline frame rate (frames per second) changed
//--------------------------------------------------------------------
//virtual
void qtTimeSlider::FrameRateChanged( float i_FrameRate )
{
}

//----------------------------------------------------------------------------
// Turn scrubbing on when mouse is down on timeline
//----------------------------------------------------------------------------
void qtTimeSlider::SliderThumbDown()
{
	tmlnTimeLine::SetIsScrubbing( true );
}
void qtTimeSlider::SliderThumbUp()
{
	tmlnTimeLine::SetIsScrubbing( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qtTimeSlider::SliderChanged()
{
	float cur_time = m_slider1->GetValue();
	float min_time = m_floatEdit_MinTime->GetTimeValue().AsSeconds();
	float max_time = m_floatEdit_MaxTime->GetTimeValue().AsSeconds();
	if(cur_time < min_time)
	{
		cur_time = min_time;
		m_slider1->SetValue(min_time);
	}
	if(cur_time > max_time)
	{
		cur_time = max_time;
		m_slider1->SetValue(max_time);
	}
	tmlnTimeUtil::AdjustTimeToFrame(cur_time);

	// the "Deferred" call will set the timeline after the
	// current render thread finishes
	tmlnTimeLine::SetTimeDeferred(maTime::FromSeconds(cur_time));
	m_slider1->SetRange(tmlnTimeLine::GetMinimum().AsSeconds(), max_time);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qtTimeSlider::CurrentTimeChanged()
{
	float cur_time = m_floatEdit_CurrentTime->GetTimeValue().AsSeconds();
	float min_time = m_floatEdit_MinTime->GetTimeValue().AsSeconds();
	float max_time = m_floatEdit_MaxTime->GetTimeValue().AsSeconds();
	if(cur_time < min_time) {cur_time = min_time; m_slider1->SetValue(min_time);}
	if(cur_time > max_time) {cur_time = max_time;m_slider1->SetValue(max_time);}
	tmlnTimeUtil::AdjustTimeToFrame(cur_time);

	// the "Deferred" call will set the timeline after the
	// current render thread finishes
	tmlnTimeLine::SetTimeDeferred(maTime::FromSeconds(cur_time));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qtTimeSlider::MaxTimeChanged()
{
	maTime max_time = m_floatEdit_MaxTime->GetTimeValue();
	if(max_time < m_floatEdit_MinTime->GetTimeValue())
		max_time = m_floatEdit_MinTime->GetTimeValue();
	tmlnTimeUtil::AdjustTimeToFrame(max_time);
	tmlnTimeLine::SetTimeRange(tmlnTimeLine::GetMinimum(), max_time);
	m_slider1->SetRange(tmlnTimeLine::GetMinimum().AsSeconds(), max_time.AsSeconds());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qtTimeSlider::MinTimeChanged()
{
	maTime min_time = m_floatEdit_MinTime->GetTimeValue();
	if(min_time >  m_floatEdit_MaxTime->GetTimeValue())
		min_time = m_floatEdit_MaxTime->GetTimeValue();
	tmlnTimeUtil::AdjustTimeToFrame(min_time);
	tmlnTimeLine::SetTimeRange(min_time, tmlnTimeLine::GetMaximum());
	m_slider1->SetRange(min_time.AsSeconds(), tmlnTimeLine::GetMaximum().AsSeconds());
}

#endif	// USE_QT
