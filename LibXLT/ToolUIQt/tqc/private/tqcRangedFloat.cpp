/****************************************************************************\
**	tqcRangedFloat.hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcRangedFloat.hpp"

#include "ToolUIQt/tqc/tqcEvent.hpp"

#ifdef QT_FINISH_PORT

BEGIN_EVENT_TABLE(tqcRangedFloat, wxPanel)
    EVT_COMMAND_SCROLL(wxID_ANY, tqcRangedFloat::OnScrollChanged)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
tqcRangedFloat::tqcRangedFloat(QWidget* i_pParent)
:	wxPanel(i_pParent, wxID_ANY),
	m_Value(0),
	m_Minimum(0),
	m_Maximum(1),
	m_NumTicks(100),
	m_Exponent(1),
	m_bExpandRange(true),
	m_bRestrictValue(false)
{		
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_pSlider = new wxSlider( this, wxID_ANY, 0, 0, 100, wxDefaultPosition, wxSize( 100,20 ), wxSL_HORIZONTAL );
	bSizer1->Add( m_pSlider, 1, wxALL, 0 );
	
	m_pFloatEdit = new tqcFloatEdit(this, wxID_ANY, wxDefaultPosition, wxSize( 54,20 ));
	this->Connect( m_pFloatEdit->GetId(), wxEVT_VALUE_CHANGED,
					wxCommandEventHandler(tqcRangedFloat::OnTextChange) );
	bSizer1->Add( m_pFloatEdit, 0, wxALL, 0 );

	// Set up font and colors
	this->InheritAttributes();
	
	this->SetSizer( bSizer1 );
	this->Layout();

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
	m_pSlider->SetRange(0, m_NumTicks);
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
	m_pFloatEdit->Show( i_bShow );
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
void tqcRangedFloat::OnTextChange(wxCommandEvent& i_Event)
{
	wxString str;
	m_Value = m_pFloatEdit->GetFloatValue();
	if(GetRestrictFlag())
	{
		if(m_Value < GetMinimum())
		{
			str << (int)GetMinimum();
			m_pFloatEdit->ChangeValue(str);
			m_Value = GetMinimum();
		}
		else if ( m_Value > GetMaximum())
		{
			str << (int)GetMaximum();
			m_pFloatEdit->ChangeValue(str);
			m_Value = GetMaximum();
		}
	}
	update_slider();
	notify_callback();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcRangedFloat::OnScrollChanged(wxScrollEvent& i_Event)
{
	float range = m_Maximum - m_Minimum;
	if (range > 0)
	{
		float val = m_pSlider->GetValue() / (float) m_NumTicks;
		val = ::powf(val, m_Exponent);
		float new_val = val * range + m_Minimum;
		if (new_val != m_Value)
		{
			m_Value = new_val;
			m_pFloatEdit->SetFloatValue(m_Value);

			notify_callback();
		}
	}
	update_slider();
	// Allow parent classes to also trap for scroll events
	i_Event.Skip();
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

	float range = m_Maximum - m_Minimum;
	if (range > 0)
	{
		float val = (m_Value - m_Minimum) / range; // convert to 0-1
        if (val < 0)
            val = 0;
        else
        {
            float exp = (m_Exponent != 0) ? (1.0 / m_Exponent) : 1;
			val = ::powf(val, exp);
            val *= m_NumTicks;
            if (val > m_NumTicks) val = m_NumTicks;
        }
        m_pSlider->SetValue((int)val);
	}
	else
		m_pSlider->SetValue(0);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void tqcRangedFloat::notify_callback()
{
	// trigger the callback
	wxCommandEvent changed_event(wxEVT_VALUE_CHANGED, this->GetId());
	m_pFloatEdit->ProcessCommand(changed_event);
}

#endif // USE_QT
