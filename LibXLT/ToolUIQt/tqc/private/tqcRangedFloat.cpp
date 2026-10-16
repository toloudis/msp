/****************************************************************************\
**	tqcRangedFloat.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"

#include <math.h>

#ifdef USE_QT

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcRangedFloat::tqcRangedFloat(QWidget* i_pParent)
:	QWidget(i_pParent),
	m_Value(0),
	m_Minimum(0),
	m_Maximum(1),
	m_NumTicks(100),
	m_Exponent(1),
	m_bExpandRange(true),
	m_bRestrictValue(false),
	m_bUpdatingSlider(false)
{
	QHBoxLayout* bSizer1 = new QHBoxLayout(this);
	bSizer1->setContentsMargins(0, 0, 0, 0);
	bSizer1->setSpacing(2);

	m_pSlider = new QSlider( Qt::Horizontal, this );
	m_pSlider->setRange(0, m_NumTicks);
	m_pSlider->setMinimumWidth(60);
	QObject::connect(m_pSlider, &QSlider::valueChanged, [this](int i_Position) { OnScrollChanged(i_Position); });
	bSizer1->addWidget( m_pSlider, 1 );

	m_pFloatEdit = new tqcFloatEdit(this);
	m_pFloatEdit->setFixedWidth(54);
	m_pFloatEdit->SetValueChangedCallback([this]() { OnTextChange(); });
	bSizer1->addWidget( m_pFloatEdit, 0 );

	update_slider();
}

//--------------------------------------------------------------------
//	Value
//--------------------------------------------------------------------
const float tqcRangedFloat::GetValue() const
{
	return m_Value;
}
void tqcRangedFloat::SetValue(const float i_Value)
{
	m_Value = i_Value;
	m_pFloatEdit->SetFloatValue(m_Value);
	update_slider();
}

//--------------------------------------------------------------------
// Set minimum and maximum in one step.
//--------------------------------------------------------------------
void tqcRangedFloat::SetRange(float i_Minimum, float i_Maximum)
{
	m_Minimum = i_Minimum;
	m_Maximum = i_Maximum;
	update_slider();
}

//--------------------------------------------------------------------
//	Minimum
//--------------------------------------------------------------------
const float tqcRangedFloat::GetMinimum() const
{
	return m_Minimum;
}
void tqcRangedFloat::SetMinimum(const float i_Minimum)
{
	m_Minimum = i_Minimum;
	update_slider();
}

//--------------------------------------------------------------------
//	Maximum
//--------------------------------------------------------------------
const float tqcRangedFloat::GetMaximum() const
{
	return m_Maximum;
}
void tqcRangedFloat::SetMaximum(const float i_Maximum)
{
	m_Maximum = i_Maximum;
	update_slider();
}

//--------------------------------------------------------------------
// Functions to check if the flag restricting the ranges are set
//--------------------------------------------------------------------
const bool tqcRangedFloat::GetRestrictFlag() const
{
	return m_bRestrictValue;
}
void tqcRangedFloat::SetRestrictFlag( const bool i_bRestrictValue)
{
	m_bRestrictValue = i_bRestrictValue;
}

//--------------------------------------------------------------------
//	DecimalPlaces - passed on to FloatEdit control
//--------------------------------------------------------------------
const short tqcRangedFloat::GetDecimalPlaces() const
{
	return m_pFloatEdit->GetDecimalPlaces();
}
void tqcRangedFloat::SetDecimalPlaces(const short i_DecimalPlaces)
{
	m_pFloatEdit->SetDecimalPlaces(i_DecimalPlaces);
}

//--------------------------------------------------------------------
//	NumTicks
//--------------------------------------------------------------------
const short tqcRangedFloat::GetNumTicks() const
{
	return m_NumTicks;
}
void tqcRangedFloat::SetNumTicks(const short i_NumTicks)
{
	m_NumTicks = i_NumTicks;
	m_bUpdatingSlider = true;
	m_pSlider->setRange(0, m_NumTicks);
	m_bUpdatingSlider = false;
	update_slider();
}

//--------------------------------------------------------------------
//	Exponent
//--------------------------------------------------------------------
const short tqcRangedFloat::GetExponent() const
{
	return m_Exponent;
}
void tqcRangedFloat::SetExponent(const short i_Exponent)
{
	m_Exponent = i_Exponent;
	update_slider();
}

//--------------------------------------------------------------------
// ShowValue turns on and off the display of the float edit control
//--------------------------------------------------------------------
void tqcRangedFloat::ShowValue(bool i_bShow)
{
	m_pFloatEdit->setVisible( i_bShow );
}

//--------------------------------------------------------------------
// If true, this flag allows the slider to expand its range to
// accomodate values outside of its minimum and maximum.
//--------------------------------------------------------------------
void tqcRangedFloat::AllowExpandRange(bool i_bExpand)
{
	m_bExpandRange = i_bExpand;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcRangedFloat::OnTextChange()
{
	m_Value = m_pFloatEdit->GetFloatValue();
	if(GetRestrictFlag())
	{
		if(m_Value < GetMinimum())
		{
			m_Value = GetMinimum();
			m_pFloatEdit->SetFloatValue(m_Value);
		}
		else if ( m_Value > GetMaximum())
		{
			m_Value = GetMaximum();
			m_pFloatEdit->SetFloatValue(m_Value);
		}
	}
	update_slider();
	NotifyValueChanged();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcRangedFloat::OnScrollChanged(int i_Position)
{
	if (m_bUpdatingSlider)
		return;

	float range = m_Maximum - m_Minimum;
	if (range > 0)
	{
		float val = i_Position / (float) m_NumTicks;
		val = ::powf(val, m_Exponent);
		float new_val = val * range + m_Minimum;
		if (new_val != m_Value)
		{
			m_Value = new_val;
			m_pFloatEdit->SetFloatValue(m_Value);
			NotifyValueChanged();
		}
	}
	update_slider();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcRangedFloat::update_slider()
{
	if (m_bExpandRange)
	{
		// Resize ranges if value is outside of range.
		// If so, expand the range past the current value somewhat - not just to the value.
		if (m_Value > m_Maximum)
			m_Maximum = m_Value + ::fabsf(m_Value);
		if (m_Value < m_Minimum)
			m_Minimum = m_Value - ::fabsf(m_Value);
	}

	int position = 0;
	float range = m_Maximum - m_Minimum;
	if (range > 0)
	{
		float val = (m_Value - m_Minimum) / range; // convert to 0-1
        if (val < 0)
            val = 0;
        else
        {
            float exp = (m_Exponent != 0) ? (1.0f / m_Exponent) : 1;
			val = ::powf(val, exp);
            val *= m_NumTicks;
            if (val > m_NumTicks) val = m_NumTicks;
        }
		position = (int)val;
	}

	if (m_pSlider->value() != position)
	{
		m_bUpdatingSlider = true;
		m_pSlider->setValue(position);
		m_bUpdatingSlider = false;
	}
}

#endif // USE_QT
