/****************************************************************************\
**	tqcNumericUpDown.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcNumericUpDown.hpp"

#ifdef USE_QT

//============================================================================
// Up and down arrows drawn like a spin box's buttons
//============================================================================
class tqcSpinArrows : public QWidget
{
public:
	tqcSpinArrows(tqcNumericUpDown* i_pOwner)
		: QWidget(i_pOwner), m_pOwner(i_pOwner), m_Direction(0),
		  m_LastY(0), m_bDragging(false)
	{
		this->setFixedWidth(16);
		this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

		// Repeat the step while an arrow is held
		m_RepeatTimer.setInterval(80);
		QObject::connect(&m_RepeatTimer, &QTimer::timeout, [this]()
		{
			if (!m_bDragging && m_Direction != 0)
				m_pOwner->step((float)m_Direction);
		});
		m_StartTimer.setSingleShot(true);
		m_StartTimer.setInterval(400);
		QObject::connect(&m_StartTimer, &QTimer::timeout, [this]() { m_RepeatTimer.start(); });
	}

protected:
	virtual void paintEvent(QPaintEvent*)
	{
		QPainter painter(this);
		QStyleOption arrow;
		arrow.initFrom(this);
		const QStyle::State normal_state = arrow.state | QStyle::State_Raised;

		// up arrow in the top half, down arrow in the bottom half
		arrow.rect = QRect(0, 0, width(), height()/2);
		arrow.state = (m_Direction > 0) ? (normal_state | QStyle::State_Sunken) : normal_state;
		style()->drawPrimitive(QStyle::PE_PanelButtonTool, &arrow, &painter, this);
		style()->drawPrimitive(QStyle::PE_IndicatorArrowUp, &arrow, &painter, this);

		arrow.rect = QRect(0, height()/2, width(), height() - height()/2);
		arrow.state = (m_Direction < 0) ? (normal_state | QStyle::State_Sunken) : normal_state;
		style()->drawPrimitive(QStyle::PE_PanelButtonTool, &arrow, &painter, this);
		style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrow, &painter, this);
	}

	virtual void mousePressEvent(QMouseEvent* i_pEvent)
	{
		if (i_pEvent->button() != Qt::LeftButton)
			return;
		m_Direction = (i_pEvent->position().y() < height()/2) ? 1 : -1;
		m_LastY = (int)i_pEvent->position().y();
		m_bDragging = false;
		m_pOwner->step((float)m_Direction);
		m_StartTimer.start();
		update();
	}

	virtual void mouseMoveEvent(QMouseEvent* i_pEvent)
	{
		if (m_Direction == 0)
			return;

		// Dragging up increases the value, dragging down decreases it.
		int y = (int)i_pEvent->position().y();
		int delta = m_LastY - y;
		if (delta != 0)
		{
			m_bDragging = true;
			m_RepeatTimer.stop();
			m_StartTimer.stop();
			m_LastY = y;
			m_pOwner->step((float)delta);
		}
	}

	virtual void mouseReleaseEvent(QMouseEvent*)
	{
		m_Direction = 0;
		m_bDragging = false;
		m_StartTimer.stop();
		m_RepeatTimer.stop();
		update();
	}

private:
	tqcNumericUpDown* m_pOwner;
	int m_Direction;	// 1 up, -1 down, 0 not pressed
	int m_LastY;
	bool m_bDragging;
	QTimer m_StartTimer;
	QTimer m_RepeatTimer;
};


//============================================================================
//============================================================================

tqcNumericUpDown* tqcNumericUpDown::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcNumericUpDown::tqcNumericUpDown(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_fValue(0.0f),
	m_Minimum(0.0f),
	m_Maximum(100),
	m_Increment(1),
	m_bRestrictValue(false)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(0);

	m_pFloatEdit = new tqcFloatEdit(this);
	m_pFloatEdit->SetValueChangedCallback([this]() { OnTextChange(); });
	bSizer1->addWidget( m_pFloatEdit, 1 );

	m_pSpinner = new tqcSpinArrows(this);
	bSizer1->addWidget( m_pSpinner, 0 );

	this->setFocusProxy(m_pFloatEdit);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcNumericUpDown::~tqcNumericUpDown()
{
	if (tqcNumericUpDown::Instance == this)
	{
		tqcNumericUpDown::Instance = NULL;
	}
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const float tqcNumericUpDown::GetValue() const
{
	return m_fValue;
}
void tqcNumericUpDown::SetValue(const float i_Value)
{
	m_fValue = i_Value;
	m_pFloatEdit->SetFloatValue(m_fValue);
}

//--------------------------------------------------------------------
//	Minimum
//--------------------------------------------------------------------
const float tqcNumericUpDown::GetMinimum() const
{
	return m_Minimum;
}
void tqcNumericUpDown::SetMinimum(const float i_Minimum)
{
	m_Minimum = i_Minimum;
}

//--------------------------------------------------------------------
//	Maximum
//--------------------------------------------------------------------
const float tqcNumericUpDown::GetMaximum() const
{
	return m_Maximum;
}
void tqcNumericUpDown::SetMaximum(const float i_Maximum)
{
	m_Maximum = i_Maximum;
}

//--------------------------------------------------------------------
// Functions to check if the flag restricting the ranges are set
//--------------------------------------------------------------------
const bool tqcNumericUpDown::GetRestrictFlag() const
{
	return m_bRestrictValue;
}
void tqcNumericUpDown::SetRestrictFlag( const bool i_bRestrictValue)
{
	m_bRestrictValue = i_bRestrictValue;
}

//--------------------------------------------------------------------
//	DecimalPlaces - passed on to FloatEdit control
//--------------------------------------------------------------------
const short tqcNumericUpDown::GetDecimalPlaces() const
{
	return m_pFloatEdit->GetDecimalPlaces();
}
void tqcNumericUpDown::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pFloatEdit->SetDecimalPlaces(i_DecimalPlaces);
}

//--------------------------------------------------------------------
//	Increment
//--------------------------------------------------------------------
const float tqcNumericUpDown::GetIncrement() const
{
	return m_Increment;
}
void tqcNumericUpDown::SetIncrement(const float i_Increment)
{
	m_Increment = i_Increment;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float tqcNumericUpDown::restrict_value(float i_Value) const
{
	if (GetRestrictFlag())
	{
		if (i_Value < GetMinimum())
			return GetMinimum();
		if (i_Value > GetMaximum())
			return GetMaximum();
	}
	return i_Value;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcNumericUpDown::OnTextChange()
{
	m_fValue = m_pFloatEdit->GetFloatValue();
	float restricted = restrict_value(m_fValue);
	if (restricted != m_fValue)
	{
		m_fValue = restricted;
		m_pFloatEdit->SetFloatValue(m_fValue);
	}
	NotifyValueChanged();
}

//----------------------------------------------------------------------------
// Change the value by a number of increments (from the arrows)
//----------------------------------------------------------------------------
void tqcNumericUpDown::step(float i_Steps)
{
	float new_value = restrict_value(m_fValue + i_Steps * m_Increment);
	if (new_value != m_fValue)
	{
		m_fValue = new_value;
		m_pFloatEdit->SetFloatValue(m_fValue);
		NotifyValueChanged();
	}
}

#endif // USE_QT
